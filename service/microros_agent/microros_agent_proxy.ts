// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


import { DeviceMethodParams } from "azure-iothub";
import dgram from 'dgram';

const { exec } = require('child_process');
const serviceSDK = require('azure-iothub');
const iothubRegistry = require('azure-iothub').Registry;
const uuid = require('uuid');
const terminate = require('terminate');
const anHourFromNow = require('azure-iot-common').anHourFromNow;
const serviceSas = require('azure-iothub').SharedAccessSignature;
const Message = require('azure-iot-common').Message;
const EventHubClient = require('@azure/event-hubs').EventHubClient;
const EventPosition = require('@azure/event-hubs').EventPosition;
const ProvisioningServiceClient = require('azure-iot-provisioning-service').ProvisioningServiceClient;


//
// isEmpty check if object is empty.
//
function isEmpty(obj) {
    for (let key in obj) {
        if (obj.hasOwnProperty(key)) {
            return false;
        }
    }

    return true;
}


//
// getEventHubDecodedMessage routine decodes the eventHub AMQ message to string
//
function getEventHubDecodedMessage(eventData): string {
    let message = ""

    if (!isEmpty(eventData.body)) {
        message += JSON.stringify(eventData.body)
    }

    if (!isEmpty(eventData.applicationProperties)) {
        message += JSON.stringify(eventData.applicationProperties)
    }

    return message
}

//
// monitorTelemetryMessageDoWork monitor telemetry message in event hub messages
//
function monitorTelemetryMessageDoWork(IoTHubConnectionString: string, port: number): Promise<number> {
    return monitorTelemetryMessage(IoTHubConnectionString, port);
}

//
// monitorTelemetryMessage monitor telemetry message in event hub messages
//
async function monitorTelemetryMessage(IoTHubConnectionString: string, port: number): Promise<number> {
    let error = null;
    const startAfterTime = Date.now() - 5000
    let client = await EventHubClient.createFromIotHubConnectionString(IoTHubConnectionString);
    let partitionIds = await client.getPartitionIds();
    let dict = {};

    let workPromise: Promise<void> = new Promise(function (resolve, reject) {
        const onEventHubMessage = function (eventData) {
            let deviceId = eventData.annotations['iothub-connection-device-id']
            if (!(deviceId in dict)) {
                console.log('New device id is: ' + deviceId)
                const udp_client = dgram.createSocket('udp4');
                udp_client.on('error', (err) => {
                    console.log(`client error:\n${err.stack}`);
                    udp_client.close();
                    resolve();
                });
                udp_client.on('message', (msg, rinfo) => {
                    console.log(`server got: ${msg} from ${rinfo.address}:${rinfo.port}`);
                });
                udp_client.connect(port);
                dict[deviceId] = udp_client;
            }
            let eventMsg = getEventHubDecodedMessage(eventData);
            console.log('eventMsg: ' + eventMsg);
            dict[deviceId].send(eventMsg)
        };
        const onEventHubError = function (err) {
            console.log(('Error from Event Hub Client Receiver: ' + err.toString()));
            resolve();
        };

        // Listing for message in all partitions
        partitionIds.forEach(function (partitionId) {
            client.receive(partitionId, onEventHubMessage, onEventHubError, {
                eventPosition: EventPosition.fromEnqueuedTime(startAfterTime)
            });
        });
    });

    // wait for work to complete
    try {
        await workPromise;
    } catch (err) {
        console.log(error)
        error = err
    }

    if (client) {
        try {
            console.log("Closing client")
            await client.close();
        } catch (err) {
            console.log(err)
        }
    }

    /* report promise result */
    if (error != null) {
        throw (error)
    }

    return 0
}


//
// getHubConnectionString returns hub's connection string, from either command line or environment
//
function getHubConnectionString(): string {
    if (argv.connectionString != null) {
        return argv.connectionString
    }
    else if (process.env.IOTHUB_CONNECTION_STRING != null) {
        return process.env.IOTHUB_CONNECTION_STRING
    }
    else {
        throw "Hub connection string must be configured in the command line or environment variable IOTHUB_CONNECTION_STRING"
    }
}

let argv = require('yargs')
    .usage('Usage: $0 --connectionString <HUB CONNECTION STRING> --port <UDP port of microROS agent>')
    .option('connectionString', {
        alias: 'c',
        describe: 'The connection string for the *IoTHub* instance',
        type: 'string',
        demandOption: false
    })
    .option('port', {
        alias: 'p',
        describe: 'The UDP port microROS agent is listening',
        type: 'number',
        demandOption: false
    })
    .argv;

// IoTHubConnectionString is the Hub connection string used to create our test device against
const IoTHubConnectionString: string = getHubConnectionString()

monitorTelemetryMessage(IoTHubConnectionString, argv.port);