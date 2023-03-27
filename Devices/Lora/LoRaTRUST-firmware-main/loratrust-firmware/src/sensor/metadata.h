#pragma once

#include <Arduino.h>

#include <ArduinoLog.h>

#include "config.h"

#include "message/messageService.h"

#include "message/messageManager.h"

#include "sensor/metadataMessage.h"

#include "sensor/metadataCommandService.h"

class Metadata: public MessageService {
public:
    static Metadata& getInstance() {
        static Metadata instance;
        return instance;
    }

    void initMetadata();

    void startMetadata();

    void stopMetadata();

    String getJSON(DataMessage* message);

    MetadataCommandService* metadataCommandService = new MetadataCommandService();

private:
    Metadata(): MessageService(MetadataApp, "Metadata") {
        commandService = metadataCommandService;
    };

    TaskHandle_t metadata_TaskHandle = NULL;

    void createMetadataTask();

    static void metadataLoop(void*);

    void createAndSendMetadata();

    bool running = true;

    void getJSONWithoutSign(DynamicJsonDocument& doc, MetadataMessage* metadataMessage);

    void signData(MetadataMessage* metadataMessage);

    uint8_t metadataId = 0;
};