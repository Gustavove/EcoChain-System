#pragma once

#include <Arduino.h>

#include "sensorMessage.h"

#pragma pack(1)
class TemperatureMessage: public SensorMessageGeneric {
public:
    float temperature;

    TemperatureMessage() {
        sensorType = SensorTemperature;
    }

    float getTemperature() {
        return temperature;
    }

    void setTemperature(float value) {
        temperature = value;
    }

    void serialize(JsonObject& doc) {
        SensorMessageGeneric* message = (SensorMessageGeneric*) this;
        message->serialize(doc);
        doc["temperature"] = temperature;
    }
};

#pragma pack()