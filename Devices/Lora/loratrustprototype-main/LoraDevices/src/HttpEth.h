#ifndef HTTPETH_H
#define HHTPETH_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "config.h"

class HttpEth {
private:
    WiFiMulti wifiMulti;
    HTTPClient http;
    char* ssid, pwd;
    String ip;
    int port, status;
public:
    HttpEth();
    void initConnection(char*, char*, String, int);
    void listNetworks();
    //Eth methods
    void getAccountBalance();
    String sendRawTransaction(String);
    void getTransactionCount();
    //void getSmartContractVariableValue();
    void getOwnerAddress();
    bool checkServerConnected();

    //Codigo Gustavo
    String sendData(String);
};

#endif