#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include <ArduinoJson.h>

#pragma pack(1)
class MetadataSensorMessage {
public:
    MetadataSensorMessage() {
    }

    void serialize(JsonObject& doc) {
        doc["type"] = type;
        doc["sample_size"] = sampleSize;
        doc["send_time_interval"] = sendTimeInterval;
        doc["calibration"] = calibration;
        doc["StDev"] = stDev;
        doc["Var"] = variance;
        doc["missing_Value"] = missingValues;
    }

    uint8_t type;
    uint8_t sampleSize;
    uint32_t sendTimeInterval;
    uint32_t calibration;
    float stDev;
    float variance;
    uint8_t missingValues;

};

#pragma pack()