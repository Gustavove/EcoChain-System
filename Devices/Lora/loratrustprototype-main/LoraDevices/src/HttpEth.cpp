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

  // print the network number and name for each network found:
  // for (int thisNet = 0; thisNet < numSsid; thisNet++) {
  //   Serial.print(thisNet);
  //   Serial.print(") ");
  //   Serial.print(WiFi.SSID(thisNet));
  //   Serial.print("\tSignal: ");
  //   Serial.print(WiFi.RSSI(thisNet));
  //   Serial.print(" dBm");
  //   //Serial.print("\tEncryption: ");
  //   //printEncryptionType(WiFi.encryptionType(thisNet));
  // }
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


///CODIGO ANTIGUO ELOY

void HttpEth::getAccountBalance() {
  //String message = "{\"jsonrpc\":\"2.0\",\"method\":\"eth_getBalance\",\"params\":[\"0xFFcf8FDEE72ac11b5c542428B35EEF5769C409f0\",\"latest\"]}";
  String message = "{\n\"method\":\"web3_clientVersion\",\n\"params\":[]\n}";
  http.begin("http://" + this->ip + ":" + String(port) + "/");
  http.addHeader("Content-Type", "application/json", true, true);
  http.addHeader("Accept", "*/*", false, true);
  Serial.println(message);
  int res = http.POST(message);

  if (res == -1) Serial.println("He intentado enviar el mensaje al servidor, pero he recibido error -1. Parece que el servidor está desconectado o no responde");
  else {
    Serial.print("He enviado el mensaje al servidor, y me ha devuelto un código HTTP ");
    Serial.println(res);
    Serial.println(http.getString());
  }
  http.end();
}

bool HttpEth::checkServerConnected() {
  String message = "{\"jsonrpc\":\"2.0\",\"method\":\"web3_clientVersion\",\"params\":[], \"id\":1}";
  http.begin("http://" + this->ip + ":" + String(port) + "/");
  http.addHeader("Content-Type", "application/json", true, true);
  http.addHeader("Accept", "*/*", false, true);
  Serial.println(message);
  int res = http.POST(message);

  if (res == -1) Serial.println("Connection to server has received an error -1. The server seams to be disconnected or not responding.");
  else {
    Serial.println("Response code: " + String(res));
    if (res == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  }
  http.end();

  return res == HTTP_CODE_OK;
}

void HttpEth::getTransactionCount() {
  HTTPClient http;
  //String message = "{\"jsonrpc\":\"2.0\",\"method\":\"eth_getBalance\",\"params\":[\"0xFFcf8FDEE72ac11b5c542428B35EEF5769C409f0\",\"latest\"]}";
  String message = "{\"jsonrpc\":\"2.0\",\"method\":\"eth_getTransactionCount\",\"params\":[\"0x90F8bf6A479f320ead074411a4B0e7944Ea8c9C1\",\"latest\"]}";
  //String message = "{\n\"method\":\"web3_clientVersion\",\n\"params\":[]\n}";
  http.begin("http://" + this->ip + ":" + String(port) + "/");
  http.addHeader("Content-Type", "application/json", true, true);
  http.addHeader("Accept", "*/*", false, true);
  Serial.println(message);
  int res = http.POST(message);
  if (res == -1) Serial.println("He intentado enviar el mensaje al servidor, pero he recibido error -1. Parece que el servidor está desconectado o no responde");
  else {
    Serial.print("He enviado el mensaje al servidor, y me ha devuelto un código HTTP ");
    Serial.println(res);
    Serial.println(http.getString());
  }
  http.end();
}

void HttpEth::getOwnerAddress() {
  String message = "{\"jsonrpc\":\"2.0\",\"method\":\"eth_call\",\"params\":[{\"from\": \"0xd8F3d56cc47C790E00da3bF3924D7E381E16557a\", \"to\": \"0x3338e23eE98325A41bEa2F4bB3702b0217f8e9E6\", \"chainId\": \"1337\", \"data\":\"0x893d20e8\"}], \"id\":1}";
  http.begin("http://" + this->ip + ":" + String(port) + "/");
  http.addHeader("Content-Type", "application/json", true, true);
  http.addHeader("Accept", "*/*", false, true);
  Serial.println(message);
  int res = http.POST(message);

  if (res == -1) Serial.println("Connection to server has received an error -1. The server seams to be disconnected or not responding.");
  else {
    Serial.println("Response code: " + String(res));
    if (res == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  }
  http.end();
}

String HttpEth::sendRawTransaction(String message) {
  http.begin("http://" + this->ip + ":" + String(port) + "/");
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

String HttpEth::sendData(String message) {
  HTTPClient http;
  http.begin("YOUR_URL");
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

