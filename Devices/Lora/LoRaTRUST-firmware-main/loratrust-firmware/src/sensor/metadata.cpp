#include "metadata.h"
#include "sensor/temperature.h"

void Metadata::initMetadata() {
    Log.notice(F("Initializing metadata" CR));
    createMetadataTask();
}

void Metadata::startMetadata() {
    Log.notice(F("Starting metadata" CR));
    running = true;
    xTaskNotifyGive(metadata_TaskHandle);
}

void Metadata::stopMetadata() {
    Log.notice(F("Stopping metadata" CR));
    running = false;
}

String Metadata::getJSON(DataMessage* message) {
    MetadataMessage* metadataMessage = (MetadataMessage*) message;
    DynamicJsonDocument doc(1024);

    getJSONWithoutSign(doc, metadataMessage);

    metadataMessage->serializeSignature(doc);

    String json;
    serializeJson(doc, json);

    return json;
}

void Metadata::getJSONWithoutSign(DynamicJsonDocument& doc, MetadataMessage* metadataMessage) {
    JsonObject data = doc.createNestedObject("data");

    metadataMessage->serialize(data);

    JsonArray sensorsArray = data.createNestedArray("sensors");

    for (int i = 0; i < metadataMessage->metadataSize; i++) {
        JsonObject sensorObject = sensorsArray.createNestedObject();

        metadataMessage->sensorMetadata[i].serialize(sensorObject);
    }
}


void Metadata::createMetadataTask() {
    int res = xTaskCreate(
        metadataLoop,
        "Metadata Task",
        4096,
        (void*) 1,
        2,
        &metadata_TaskHandle
    );

    if (res != pdPASS) {
        Log.error(F("Failed to create metadata task" CR));
    }
}

void Metadata::metadataLoop(void* pvParameters) {
    Log.notice(F("Metadata task started" CR));

    Metadata& metadata = Metadata::getInstance();

    while (true) {
        if (!metadata.running)
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        else {
            metadata.createAndSendMetadata();

            vTaskDelay(METADATA_UPDATE_DELAY / portTICK_PERIOD_MS);
        }
    }
}

void Metadata::createAndSendMetadata() {
    uint8_t metadataSize = 1; //TODO: This should be dynamic with an array of sensors
    uint16_t metadataSensorSize = metadataSize * sizeof(MetadataSensorMessage);
    uint16_t messageWithHeaderSize = sizeof(MetadataMessage) + metadataSensorSize;

    MetadataMessage* message = (MetadataMessage*) malloc(messageWithHeaderSize);

    if (message) {

        message->appPortDst = appPort::WiFiApp;
        message->appPortSrc = appPort::MetadataApp;
        message->addrSrc = LoraMesher::getInstance().getLocalAddress();
        message->addrDst = 0;
        message->messageId = metadataId++;

        message->messageSize = messageWithHeaderSize - sizeof(DataMessageGeneric);
        GPSMessage gpsMessage = GPSService::getInstance().getGPSMessage();
        memcpy(&message->gps, &gpsMessage, sizeof(GPSMessage));
        message->metadataSendTimeInterval = METADATA_UPDATE_DELAY;

        message->metadataSize = metadataSize;

        //TODO: This should be a vector of sensors
        Temperature& temperature = Temperature::getInstance();

        MetadataSensorMessage* tempMetadata = temperature.getMetadataMessage();
        memcpy(message->sensorMetadata, tempMetadata, sizeof(MetadataSensorMessage));

        signData(message);

        MessageManager::getInstance().sendMessage(messagePort::WiFiPort, (DataMessage*) message);

        free(message);
    }
    else {
        Log.errorln(F("Failed to allocate memory for metadata message"));
    }
}

void Metadata::signData(MetadataMessage* metadataMessage) {
    DynamicJsonDocument doc(1024);

    getJSONWithoutSign(doc, metadataMessage);
    String json;
    serializeJson(doc, json);

    String signStr = Wallet::getInstance().sign(json);
    memcpy(metadataMessage->signature, signStr.c_str(), signStr.length());
}
