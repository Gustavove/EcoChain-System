#ifndef LoraNode_H
#define LoraNode_H

#include <Arduino.h>
#include <SPI.h>             
#include <LoRa.h>
#include "config.h"

class LoraNode {

    private:
        static void onTxDone();
        static void onReceive(int packetSize);
        void LoRa_sendMessage(String message);
        static void LoRa_txMode();
        static void LoRa_rxMode();
        bool sending;
        int messageNumber;
        String nodeId;
    public:
        LoraNode();
        void begin(String);
        bool sendMessage(String);
};

#endif