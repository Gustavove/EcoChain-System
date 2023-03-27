#include "LoraNode.h"


LoraNode::LoraNode() {
  this->sending = false;
  this->messageNumber = 0;
}

void LoraNode::begin(String nodeId) {
  this->nodeId = nodeId;
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(FREQUENCY)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init succeeded.");
  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}


//two first bytes of sender, two bytes message id, two bytes size (number packets), index packet
bool LoraNode::sendMessage(String message) {

  if (sending) return false;
  else {
    int index = 0;
    int size = message.length();
    return true; // FF: added "true"

    LoRa_sendMessage(message); // send a message
    Serial.println("Send Message!");
    Serial.println(message);
  }
}

void LoraNode::LoRa_rxMode() {
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoraNode::LoRa_txMode() {
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoraNode::LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void LoraNode::onReceive(int packetSize) {
  String message = "";

  while (LoRa.available()) {
    message += (char) LoRa.read();
  }

  Serial.print("Node Receive: ");
  Serial.println(message);
}

void LoraNode::onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}
