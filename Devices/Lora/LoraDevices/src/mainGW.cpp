#include <Arduino.h>
#include "config.h"

#ifndef NODE

#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "HttpEth.h"

HttpEth httpEth;

void LoRa_rxMode() {
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode() {
  LoRa.idle();                          // set standby mode
  LoRa.enableInvertIQ();                // active invert I and Q signals
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onReceive(int packetSize) {
  String message = "";

  while (LoRa.available()) {
    message += (char) LoRa.read();
  }

  Serial.print("Gateway Receive: ");
  Serial.println(message);
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
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

void initLora() {
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(FREQUENCY)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Simple Gateway");
  Serial.println("Only receive messages from nodes");
  Serial.println("Tx: invertIQ enable");
  Serial.println("Rx: invertIQ disable");
  Serial.println();

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}

void loopLora() {
  // if (runEvery(5000)) { // repeat every 5000 millis
  String message = "HeLoRa World! ";
  message += "I'm a Gateway! ";
  message += millis();

  LoRa_sendMessage(message); // send a message

  Serial.println("Send Message!");
  // }
}

void setup() {
  Serial.begin(115200);                   // initialize serial
  while (!Serial);

  // initLora();

  httpEth.initConnection(RUT_SSID, WIFIPWD, SERVER_IP, SERVER_PORT);

  httpEth.getOwnerAddress();
}

bool first = true;
void loop() {

  // loopLora();

  vTaskDelay(5000 / portTICK_RATE_MS); //Wait 5 seconds

  if (first) {
    first = false;
    // int nonce = 0;
    // int price = 50;
    // int limit = 21000;
    // int amount = 1;
    // String data = functionCallToData("transfer(address,uint256)", "337c67618968370907da31dAEf3020238D01c9de", (unsigned long long) 18446744073709551615);
    // String trans = buildTransaction(SC_ADDRESS, wallet.getPrivateKey(), nonce, price, limit, amount, data);
  }
}

#endif