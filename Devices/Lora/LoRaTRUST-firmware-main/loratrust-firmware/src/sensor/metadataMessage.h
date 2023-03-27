#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include "message/dataMessage.h"

#include "gps/gpsMessage.h"

#include "sensor/metadataSensorMessage.h"

#include "sensor/signatureMessage.h"

#pragma pack(1)
class MetadataMessage final: public DataMessageGeneric, public SignatureMessage {
public:
    MetadataMessage() {
    }

    GPSMessage gps;
    int metadataSendTimeInterval;
    uint8_t metadataSize;
    MetadataSensorMessage sensorMetadata[];

    void serialize(JsonObject& doc) {
        // Call the base class serialize function
        ((DataMessageGeneric*) (this))->serialize(doc);

        // Add the GPS data to the JSON object
        gps.serialize(doc);

        // // Add the derived class data to the JSON object
        // doc["sensorType"] = sensorType;

        // Add the derived class data to the JSON object
        doc["metadata_send_time_interval"] = metadataSendTimeInterval;
    }
};

#pragma pack()
