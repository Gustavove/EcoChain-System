#include <Arduino.h>

//Configuration
#include "config.h"

//Log
#include "ArduinoLog.h"

//Helpers
#include "helpers/helper.h"

//LoRaChat
#include "loraChat/loraChatService.h"

//Manager
#include "message/messageManager.h"

//Display
#include "display.h"

//LoRaMesh
#include "loramesh/loraMeshService.h"

//GPS libraries
#include "gps/gpsService.h"

//Bluetooth
#include "bluetooth/bluetoothService.h"

//WiFi
#include "wifi/wifiServerService.h"

//Wallet
#include "wallet/Wallet.h"

//Temperature
#include "sensor/temperature.h"

//Metadata
#include "sensor/metadata.h"

#pragma region Wallet

Wallet& wallet = Wallet::getInstance();

void initWallet() {
    wallet.begin();
}

#pragma endregion

#pragma region Metadata

Metadata& metadata = Metadata::getInstance();

void initMetadata() {
    metadata.initMetadata();
}

#pragma endregion

#pragma region Temperature
Temperature& temperature = Temperature::getInstance();

void initTemperature() {
    temperature.initTemperature();
}

#pragma endregion

#pragma region WiFi

WiFiServerService& wiFiService = WiFiServerService::getInstance();

void initWiFi() {
    wiFiService.initWiFi();
}

#pragma endregion

#pragma region LoRaMesher

LoRaMeshService& loraMeshService = LoRaMeshService::getInstance();

void initLoRaMesher() {
    //Init LoRaMesher
    loraMeshService.initLoraMesherService();
}

#pragma endregion

#pragma region LoRaChat

LoRaChatService& loraChatService = LoRaChatService::getInstance();

void initLoRaChat() {
    //Init LoRaChat
    loraChatService.initLoRaChatService();
}

#pragma endregion

#pragma region GPS

#ifdef GPS_ENABLED
#define UPDATE_GPS_DELAY 10000 //ms

GPSService& gpsService = GPSService::getInstance();

void initGPS() {
    //Initialize GPS
    gpsService.initGPS();
}
#endif

#pragma endregion

#pragma region SerialBT

BluetoothService& bluetoothService = BluetoothService::getInstance();

void initBluetooth() {
    bluetoothService.initBluetooth(String(loraMeshService.getDeviceID()));
}

#pragma endregion

#pragma region Manager

MessageManager& manager = MessageManager::getInstance();

void initManager() {
    manager.init();
    Log.verboseln("Manager initialized");

    manager.addMessageService(&bluetoothService);
    Log.verboseln("Bluetooth service added to manager");

#ifdef GPS_ENABLED
    manager.addMessageService(&gpsService);
    Log.verboseln("GPS service added to manager");
#endif

    manager.addMessageService(&loraMeshService);
    Log.verboseln("LoRaMesher service added to manager");

#ifdef LORACHAT_ENABLED
    manager.addMessageService(&loraChatService);
    Log.verboseln("LoRaChat service added to manager");
#endif

    manager.addMessageService(&wiFiService);
    Log.verboseln("WiFi service added to manager");

    manager.addMessageService(&wallet);
    Log.verboseln("Wallet service added to manager");

#ifdef METADATA_ENABLED
    manager.addMessageService(&metadata);
    Log.verboseln("Metadata service added to manager");
#endif

#ifdef TEMPERATURE_ENABLED
    manager.addMessageService(&temperature);
    Log.verboseln("Temperature service added to manager");
#endif

    Serial.println(manager.getAvailableCommands());
}

#pragma endregion

#pragma region Display

TaskHandle_t display_TaskHandle = NULL;

#define DISPLAY_TASK_DELAY 50 //ms
#define DISPLAY_LINE_TWO_DELAY 10000 //ms
#define DISPLAY_LINE_THREE_DELAY 50000 //ms
#define DISPLAY_LINE_FOUR_DELAY 20000 //ms
#define DISPLAY_LINE_FIVE_DELAY 10000 //ms
#define DISPLAY_LINE_SIX_DELAY 10000 //ms

void display_Task(void* pvParameters) {

    uint32_t lastLineTwoUpdate = 0;
    uint32_t lastLineThreeUpdate = 0;
#ifdef GPS_ENABLED
    uint32_t lastGPSUpdate = 0;
#endif
    uint32_t lastLineFourUpdate = 0;
    uint32_t lastLineFiveUpdate = 0;
    uint32_t lastLineSixUpdate = 0;
    uint32_t lastLineSevenUpdate = 0;

    while (true) {
        //Update line two every DISPLAY_LINE_TWO_DELAY ms
        if (millis() - lastLineTwoUpdate > DISPLAY_LINE_TWO_DELAY) {
            lastLineTwoUpdate = millis();
            String lineTwo = String(loraMeshService.getDeviceID(), 16);

            if (loraMeshService.isGateway())
                lineTwo += " | " + wiFiService.getIP();

            Screen.changeLineTwo(lineTwo);
        }

#ifdef GPS_ENABLED
        //Update line three every DISPLAY_LINE_THREE_DELAY ms
        // if (millis() - lastLineThreeUpdate > DISPLAY_LINE_THREE_DELAY) {
        //     lastLineThreeUpdate = millis();
        //     String lineThree = gpsService.getGPSUpdatedWait();
        //     if (lineThree.begin() != "G")
        //         Screen.changeLineThree(lineThree);
        // }

        //Update GPS every UPDATE_GPS_DELAY ms
        if (millis() - lastGPSUpdate > UPDATE_GPS_DELAY) {
            lastGPSUpdate = millis();
            String lineThree = gpsService.getGPSUpdatedWait();
            if (lineThree.startsWith("G") != 1)
                Screen.changeLineThree(lineThree);
        }
#endif

        //Update line 4 when a new message is received
        if (millis() - lastLineFourUpdate > DISPLAY_LINE_FOUR_DELAY && loraMeshService.lastReceivedFrom != 0) {
            lastLineFourUpdate = millis();
            String lineFour = "Last from: " + String(loraMeshService.lastReceivedFrom, 16);
            Screen.changeLineFour(lineFour);
        }

        //Update line 5 when a new message is sent
        if (millis() - lastLineFiveUpdate > DISPLAY_LINE_FIVE_DELAY && loraMeshService.lastSentTo != 0) {
            lastLineFiveUpdate = millis();
            String lineFive = "Last to: " + String(loraMeshService.lastSentTo, 16);
            Screen.changeLineFive(lineFive);
        }

        //Update line 6 every DISPLAY_LINE_SIX_DELAY ms
        if (millis() - lastLineSixUpdate > DISPLAY_LINE_SIX_DELAY) {
            lastLineSixUpdate = millis();
            String lineSix = "N. Sent: " + String(loraMeshService.numSent());
            Screen.changeLineSix(lineSix);
        }

        if (millis() - lastLineSevenUpdate > DISPLAY_LINE_SIX_DELAY) {
            lastLineSevenUpdate = millis();
            String lineSeven = "N. Received: " + String(loraMeshService.numReceived());
            Screen.changeLineSeven(lineSeven);
        }

        Screen.drawDisplay();
        vTaskDelay(DISPLAY_TASK_DELAY / portTICK_PERIOD_MS);
    }
}

void createUpdateDisplay() {
    int res = xTaskCreate(
        display_Task,
        "Display Task",
        4096,
        (void*) 1,
        2,
        &display_TaskHandle);
    if (res != pdPASS) {
        Log.errorln(F("Display Task creation gave error: %d"), res);
    }
}

void initDisplay() {
    Screen.initDisplay();
    createUpdateDisplay();
}

#pragma endregion

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Initialize Log
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);

    // Initialize Manager
    initManager();

#ifdef GPS_ENABLED
    // Initialize GPS
    initGPS();
#endif

    // Initialize LoRaMesh
    initLoRaMesher();

    // Initialize Bluetooth
    initBluetooth();

#ifdef LORACHAT_ENABLED
    // Initialize LoRaChat
    initLoRaChat();
#endif

    // Initialize WiFi
    initWiFi();

    //Initialize Wallet
    initWallet();

    // Initialize Metadata
#ifdef METADATA_ENABLED
    initMetadata();
#endif

#ifdef TEMPERATURE_ENABLED
    // Initialize Temperature
    initTemperature();
#endif

    // Initialize Display
    initDisplay();

    // Blink 2 times to show that the device is ready
    Helper::ledBlink(2, 100);
}

void loop() {
    //Suspend this task
    vTaskSuspend(NULL);
}