#include "temperature.h"

void Temperature::initTemperature() {
    oneWire = OneWire(ONE_WIRE_BUS);
    sensors = DallasTemperature(&oneWire);

    sensors.begin();

    createTemperatureTask();

    Log.noticeln(F("Temperature initialized"));

    startTemperature();
}

void Temperature::startTemperature() {
    running = true;
    xTaskNotifyGive(temperature_TaskHandle);

    Log.noticeln(F("Temperature task started"));
}

void Temperature::stopTemperature() {
    running = false;

    Log.noticeln(F("Temperature task stopped"));
}

float Temperature::readValue() {
    return readValueWait(0);
}

float Temperature::readValueWait(uint8_t retries) {
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);

    if (temperature == DEVICE_DISCONNECTED_C) {
        if (retries > 0) {
            return readValueWait(retries - 1);
        }
        else {
            return DEVICE_DISCONNECTED_C;
        }
    }

    //Add the new value to the array
    previousValues[previousValuesIndex] = temperature;
    previousValuesIndex = (previousValuesIndex + 1) % STORED_SENSOR_DATA;

    readValues++;

    return temperature;
}

String Temperature::getJSON(DataMessage* message) {
    TemperatureMessage* temperatureMessage = (TemperatureMessage*) message;

    DynamicJsonDocument doc(1024);

    getJSONWithoutSign(doc, temperatureMessage);

    temperatureMessage->serializeSignature(doc);

    String json;
    serializeJson(doc, json);

    return json;
}

void Temperature::getJSONWithoutSign(DynamicJsonDocument& doc, TemperatureMessage* message) {
    JsonObject data = doc.createNestedObject("data");

    message->serialize(data);
}

void Temperature::createTemperatureTask() {
    int res = xTaskCreate(
        temperatureLoop,
        "Temperature Task",
        4096,
        (void*) 1,
        2,
        &temperature_TaskHandle);
    if (res != pdPASS) {
        Log.errorln(F("Temperature task handle error: %d"), res);
    }
}

void Temperature::temperatureLoop(void*) {
    Temperature& temperature = Temperature::getInstance();

    while (true) {
        if (!temperature.running)
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        else {

            float value = temperature.readValue();

            if (value != DEVICE_DISCONNECTED_C) {
                Log.noticeln(F("Temperature: %f"), value);

                float var = temperature.getVariance();
                Log.noticeln(F("Temperature variance: %f"), var);
            }
            else {
                Log.errorln(F("Temperature reading error"));
            }

            temperature.sendTemperature(value);

            vTaskDelay(temperature.readEveryMs / portTICK_PERIOD_MS);
        }
    }
}


void Temperature::sendTemperature(float value) {
    TemperatureMessage* message = getTemperatureForSend(value);
    MessageManager::getInstance().sendMessage(messagePort::WiFiPort, (DataMessage*) message);
    delete message;
}

TemperatureMessage* Temperature::getTemperatureForSend(float value) {

    TemperatureMessage* temperatureMessage = new TemperatureMessage();

    temperatureMessage->setTemperature(value);

    temperatureMessage->appPortDst = appPort::WiFiApp;
    temperatureMessage->appPortSrc = appPort::TemperatureSensorApp;
    temperatureMessage->addrSrc = LoraMesher::getInstance().getLocalAddress();
    temperatureMessage->addrDst = 0;
    temperatureMessage->messageId = sensorMessageId++;

    temperatureMessage->sensorType = DataSensorType::SensorTemperature;
    temperatureMessage->gps = GPSService::getInstance().getGPSMessage();

    temperatureMessage->temperature = value;

    temperatureMessage->messageSize = sizeof(TemperatureMessage) - sizeof(DataMessageGeneric);

    signData(temperatureMessage);

    return temperatureMessage;
}

void Temperature::signData(TemperatureMessage* message) {
    DynamicJsonDocument doc(1024);
    getJSONWithoutSign(doc, message);

    String json;
    serializeJson(doc, json);

    String signStr = Wallet::getInstance().sign(json);
    memcpy(message->signature, signStr.c_str(), signStr.length());
}
