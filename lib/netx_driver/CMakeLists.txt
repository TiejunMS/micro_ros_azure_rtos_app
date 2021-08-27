# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

if(DEFINED PCAP_NAME)
    set(SOURCES
        nx_pcap_network_driver.c
    )
else()
    set(SOURCES
        inventek/es_wifi.c
        inventek/es_wifi_io.c
        inventek/wifi.c

        nx_wifi.c
    )
endif()

set(PROJECT netx_driver)

add_library(${PROJECT} OBJECT
    ${SOURCES}
)

if(DEFINED PCAP_NAME)
    target_compile_definitions(${PROJECT} PUBLIC -DNX_PCAP_SOURCE_NAME=\"${PCAP_NAME}\")
endif()

target_link_libraries(${PROJECT}
    PUBLIC
        azrtos::threadx
        azrtos::netxduo
)
if(DEFINED PCAP_NAME)
target_link_libraries(${PROJECT}
    PUBLIC
        pcap
)
else()
target_link_libraries(${PROJECT}
    PUBLIC
        stm32cubel4
)
endif()

if(NOT DEFINED PCAP_NAME)
    target_include_directories(${PROJECT}
        PUBLIC
            inventek
    )
endif()
