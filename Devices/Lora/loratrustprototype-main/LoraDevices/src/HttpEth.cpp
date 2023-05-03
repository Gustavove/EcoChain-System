#include "HttpEth.h"

HttpEth::HttpEth() {

}


void HttpEth::listNetworks() {
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);
  
}

void HttpEth::initConnection(char* ssid, char* pwd, String ip, int port) {
  this->ip = ip;
  this->port = port;

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }


  wifiMulti.addAP(ssid, pwd);


  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Connecting to Wifi ...");
    Serial.println(ssid);
    Serial.println(pwd);

    delay(1000);
  }
  Serial.println("\nConnected to Wifi!");
  Serial.print("\nIP address:\t");
  Serial.println(WiFi.localIP());
}

String HttpEth::sendData(String message) {
  HTTPClient http;
  http.begin("https://putus.serveo.net/sensor/adddata");
  http.addHeader("User-Agent", "LoRA/1.0", true, true);
  http.addHeader("Content-Type", "application/json", true, true);
  Serial.println(message);
  int res = http.POST(message);

  if (res == -1) Serial.println("Connection to server has received an error -1. The server seams to be disconnected or not responding.");
  else {
    Serial.println("Response code: " + String(res));
    if (res == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
      http.end();
      return payload;
    }
  }

  http.end();
  return "";
}

