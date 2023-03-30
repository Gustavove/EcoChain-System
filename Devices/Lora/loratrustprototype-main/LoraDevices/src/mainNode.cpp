#include <Arduino.h>
#include "config.h"

#ifdef NODE
#include "string.h"
#include "HttpEth.h"
#include "Wallet.h"
#include "Utils.h"
#include "OLED.h"
#include "LoraNode.h"

HttpEth httpEth;
LoraNode lora;
OLED display;

void setup() {
    Serial.begin(115200);
    Serial.println("Execution started");

    wallet.begin();
    Serial.print("pkey: ");
    Serial.println(wallet.getAddress());

    lora.begin(wallet.getAddress().substring(0, 4));

    display.begin();
    display.scrollText(WiFi.macAddress());
    
    Serial.print("starting wifi connection: ");

    httpEth.listNetworks();
    httpEth.initConnection((char*)RUT_SSID, (char*)WIFIPWD, SERVER_IP, SERVER_PORT);

    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
}

boolean runEvery(unsigned long interval) {
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        return true;
    }
    return false;
}

int num = 4;

void loop() {

    char cadena[5];
    
    if (runEvery(SENDINGTIME)) { // repeat every 1000 millis
        httpEth.sendData("{\"gps\": {\"latitude\": \"41.3894451\",\"longitude\": \"2.1111765\",\"altitude\": \"150.44\"},\"mac\":\"" + WiFi.macAddress() + "\",\"message\": {\"pH\":\""+sprintf(cadena, "%d", num)+"\",\"tds\": \"100\"},\"provider\": \"0x21e517bf6De93b1D783fEB84ddE42F589d845CEB\"}");
    }
    num++;
}

#endif