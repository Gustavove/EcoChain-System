#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include <ArduinoJson.h>

#include "helpers/helper.h"

#pragma pack(1)

class SignatureMessage {
public:
    uint8_t signature[64];

    void serializeSignature(DynamicJsonDocument& doc) {
        // Add the derived class data to the JSON object
        doc["signature"] = signature;
    }

};

#pragma pack()