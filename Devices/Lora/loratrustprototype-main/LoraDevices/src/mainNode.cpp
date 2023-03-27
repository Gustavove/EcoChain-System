
#include <Arduino.h>
#include "config.h"

#ifdef NODE
#include "Transaction.h"
#include "HttpEth.h"
#include "Wallet.h"
#include "Utils.h"
#include "OLED.h"
#include "LoraNode.h"

HttpEth httpEth;
LoraNode lora;
OLED display;

String buildTransaction(String to_add, String private_key, int nonce, int price, int limit, int amount, String functionName) {
    Transaction tx(nonce, price, limit, to_add, amount, functionName);
    tx.sign(private_key);
    return tx.getSigned();
}

String buildGetterTransaction(char* from_add, char* to_add, String data, String private_key) {
    Transaction tx(from_add, to_add, data);
    tx.sign(private_key);
    return tx.getSigned();
}

String functionCallToData(String functionName, String par1, unsigned long long par2) {
    String aux1 = Utils::pad32Bytes(par1);
    String aux2 = Utils::pad32Bytes(Utils::longDecimalToHexString(par2));
    uint8_t buf[50];
    functionName.getBytes(buf, 50);
    return Utils::hash(buf, functionName.length()).substring(0, 8) + aux1 + aux2;
}

String functionGetValue(String functionName) {
    uint8_t buf[50];
    functionName.getBytes(buf, 50);
    return Utils::hash(buf, functionName.length()).substring(0, 8);
}

String getJSONRPC(String method, String params) {
    return "{\"jsonrpc\": \"2.0\", \"method\": \"" + method + "\", \"params\": [" + params + "], \"id\":1}";
}

String getParamsWithData(String data) {
    return "{\"from\": \"" + String(MY_ADDRESS) + "\", \"to\": \"" + String(SC_ADDRESS) + "\", \"chainId\": \"" + String(CHAIN_ID) + "\", \"data\": \"0x" + data + "\"}";
}

String getCallJSON(String functionName) {
    String parsedFunctionName = functionGetValue(functionName);
    String params = getParamsWithData(parsedFunctionName);
    return getJSONRPC("eth_call", params);
}

String getRawTransactionJSONRPC(String params) {
    return getJSONRPC("eth_sendRawTransaction", params);
}

String getTransactionCount(String address) {
    return getJSONRPC("eth_getTransactionCount", "\"" + address + "\", \"latest\"");
}

String getFunctionCallToDataTest(String functionName) {
    String address = String(MY_ADDRESS);

    //Remove the 0x from the address
    address.remove(0, 2);

    String parsedAddress = Utils::pad32Bytes(address);
    String parsedFunctionName = functionGetValue(functionName) + parsedAddress;
    String params = getParamsWithData(parsedFunctionName);
    return getJSONRPC("eth_call", params);
}

String getResultFromJSON(String json) {
    int index = json.indexOf("result");
    int finalIndex = json.indexOf("\"}");
    String result = json.substring(index + 9, finalIndex);
    return result;
}

int getNonce(String address) {
    String json = httpEth.sendRawTransaction(getTransactionCount(address));
    String result = getResultFromJSON(json);
    result.remove(0, 2);
    return Utils::hexStringToInt(result);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Execution started");

    wallet.begin();
    Serial.print("pkey: ");
    Serial.println(wallet.getAddress());

    display.begin();
    display.scrollText(wallet.getAddress());

    lora.begin(wallet.getAddress().substring(0, 4));
    
    Serial.print("starting wifi connection: ");

    httpEth.listNetworks();
    httpEth.initConnection(RUT_SSID, WIFIPWD, SERVER_IP, SERVER_PORT);
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

void loop() {

    if (runEvery(SENDINGTIME)) { // repeat every 1000 millis

        httpEth.sendData("{\"gps\": {\"latitude\": \"41.3894451\",\"longitude\": \"2.1111765\",\"altitude\": \"150.44\"},\"mac\": \"00:1B:54:11:3A:B8\",\"message\": {\"pH\": \"5\",\"tds\": \"100\"},\"provider\": \"0x21e517bf6De93b1D783fEB84ddE42F589d845CEB\"}");
    }

}

#endif