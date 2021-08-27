/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "nx_api.h"
#include "nx_secure_tls_api.h"
#include "nxd_dns.h"
#include "nxd_dhcp_client.h"

#define THREADX_PACKET_COUNT 20
#define THREADX_PACKET_SIZE  1536
#define THREADX_POOL_SIZE    ((THREADX_PACKET_SIZE + sizeof(NX_PACKET)) * THREADX_PACKET_COUNT)
#define THREADX_IP_STACK_SIZE (2048)
#define THREADX_ARP_CACHE_SIZE (512)

static UCHAR threadx_ip_pool[THREADX_POOL_SIZE];
static ULONG threadx_ip_stack[THREADX_IP_STACK_SIZE / sizeof(ULONG)];
static ULONG threadx_arp_cache_area[THREADX_ARP_CACHE_SIZE / sizeof(ULONG)];

NX_IP nx_ip;
NX_PACKET_POOL nx_pool;
NX_DNS nx_dns_client;
NX_DHCP nx_dhcp_client;

void _nx_pcap_network_driver(struct NX_IP_DRIVER_STRUCT* driver_req);

// Print IPv4 address
static void print_address(CHAR* preable, uint8_t address[4])
{
    printf("\t%s: %d.%d.%d.%d\r\n", preable, address[0], address[1], address[2], address[3]);
}

static UINT dns_create()
{
    UINT status;
    ULONG dns_server_address[2];
    UINT dns_server_address_size = sizeof(dns_server_address);

    printf("Initializing DNS client\r\n");

    status = nx_dns_create(&nx_dns_client, &nx_ip, (UCHAR*)"DNS Client");
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to create DNS (%0x02)\r\n", status);
        return status;
    }

    // Use the packet pool here
#ifdef NX_DNS_CLIENT_USER_CREATE_PACKET_POOL
    status = nx_dns_packet_pool_set(&nx_dns_client, nx_ip.nx_ip_default_packet_pool);
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to create DNS packet pool (%0x02)\r\n", status);
        nx_dns_delete(&nx_dns_client);
        return status;
    }
#endif

    if (nx_dhcp_interface_user_option_retrieve(&nx_dhcp_client, 0, NX_DHCP_OPTION_DNS_SVR,
                                               (UCHAR *)(dns_server_address),
                                               &dns_server_address_size))
    {
        printf("ERROR: Failed to fetch DNS\r\n");
        nx_dns_delete(&nx_dns_client);
        return NX_NOT_SUCCESSFUL;
    }

    // Add an IPv4 server address to the Client list.
    status = nx_dns_server_add(
        &nx_dns_client, dns_server_address[0]);
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to add dns server (%0x02)\r\n", status);
        nx_dns_delete(&nx_dns_client);
        return status;
    }

    // Output DNS Server address
    print_address("DNS address", (uint8_t *)dns_server_address[0]);

    printf("SUCCESS: DNS client initialized\r\n\r\n");

    return NX_SUCCESS;
}

static void dhcp_wait()
{
ULONG   actual_status;

    printf("DHCP In Progress...\r\n");

    /* Create the DHCP instance.  */
    nx_dhcp_create(&nx_dhcp_client, &nx_ip, "DHCP Client");

    /* Start the DHCP Client.  */
    nx_dhcp_start(&nx_dhcp_client);

    /* Wait util address is solved. */
    nx_ip_status_check(&nx_ip, NX_IP_ADDRESS_RESOLVED, &actual_status, NX_WAIT_FOREVER);
}

int pcap_network_init()
{
    UINT status;

    // Initialize the NetX system
    nx_system_initialize();

    // Create a packet pool
    status =
        nx_packet_pool_create(&nx_pool, "NetX Packet Pool", THREADX_PACKET_SIZE, threadx_ip_pool, THREADX_POOL_SIZE);
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Packet pool create fail.\r\n");
        return status;
    }

    // Create an IP instance
    status = nx_ip_create(&nx_ip, "NetX IP Instance 0", 0, 0xFFFFFFFF, &nx_pool, _nx_pcap_network_driver,
                          (UCHAR *)threadx_ip_stack, sizeof(threadx_ip_stack), 0);
    if (status != NX_SUCCESS)
    {
        nx_packet_pool_delete(&nx_pool);
        printf("ERROR: IP create fail.\r\n");
        return status;
    }

    // Initialize NetX
    if (nx_arp_enable(&nx_ip, (VOID *)threadx_arp_cache_area, sizeof(threadx_arp_cache_area)) ||
        nx_icmp_enable(&nx_ip) ||
        nx_tcp_enable(&nx_ip) ||
        nx_udp_enable(&nx_ip))
    {
        nx_ip_delete(&nx_ip);
        nx_packet_pool_delete(&nx_pool);
        printf("ERROR: NetX initialize fail.\r\n");
        return status;
    }

    // Initialize TLS
    nx_secure_tls_initialize();

    // Wait DHCP
    dhcp_wait();

    // Create DNS
    status = dns_create();
    if (status != NX_SUCCESS)
    {
        nx_ip_delete(&nx_ip);
        nx_packet_pool_delete(&nx_pool);
        printf("ERROR: DNS create fail.\r\n");
        return status;
    }

    return NX_SUCCESS;
}
