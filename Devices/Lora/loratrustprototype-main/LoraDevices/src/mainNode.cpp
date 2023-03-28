
#include <Arduino.h>
#include "config.h"

#ifdef NODE
#include "HttpEth.h"
#include "Wallet.h"
#include "Utils.h"
#include "OLED.h"
#include "LoraNode.h"
#include "uECC.h"
#include <string.h>
#include <keccak.h>
#include <mbedtls/sha256.h>

HttpEth httpEth;
LoraNode lora;
OLED display;

void setup() {
    Serial.begin(115200);
    Serial.println("Execution started");

    //TEST FIRMA

    uint8_t privateKey[32] = {0xee,0x8c,0x2c,0xf0, 0x8d, 0x19, 0x37, 0x4b, 0xeb, 0x3a, 0xa9, 0xcd, 0xa9, 0xfa, 0x53, 0xaf, 0xe2, 0x68,0xdd, 0x91, 0xb5, 0xb1, 0x5a, 0xe5, 0xf5, 0x64, 0x5a, 0x20, 0x26, 0xf1, 0x26, 0x7d};
    uint8_t publicKey[64];
    const struct uECC_Curve_t* curve = uECC_secp256k1();
    uECC_compute_public_key(privateKey, publicKey, curve);
    char message[] = "Hola";

    mbedtls_sha256_context sha_ctx;
    unsigned char message_hash[32];
    mbedtls_sha256_init(&sha_ctx);
    mbedtls_sha256_update(&sha_ctx, (const unsigned char *)message, strlen(message));
    mbedtls_sha256_finish(&sha_ctx, message_hash);
    mbedtls_sha256_free(&sha_ctx);

    uint8_t signature[64];
    //Firma no determinista, se utiliza un valor aleatorio para obtener la firma
    uECC_sign(privateKey, message_hash, sizeof(message_hash), signature, curve);


    if (uECC_verify(publicKey, message_hash, sizeof(message_hash), signature, curve)) {
        Serial.println("La firma es válida");
    } else {
         Serial.println("La firma es inválida");
    }

    //WIFI

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
    
    uint8_t privateKey[32] = {0xee,0x8c,0x2c,0xf0, 0x8d, 0x19, 0x37, 0x4b, 0xeb, 0x3a, 0xa9, 0xcd, 0xa9, 0xfa, 0x53, 0xaf, 0xe2, 0x68,0xdd, 0x91, 0xb5, 0xb1, 0x5a, 0xe5, 0xf5, 0x64, 0x5a, 0x20, 0x26, 0xf1, 0x26, 0x7d};
    uint8_t publicKey[64];
    const struct uECC_Curve_t* curve = uECC_secp256k1();
    uECC_compute_public_key(privateKey, publicKey, curve);
    char message[] = "Hola";

    mbedtls_sha256_context sha_ctx;
    unsigned char message_hash[32];
    mbedtls_sha256_init(&sha_ctx);
    mbedtls_sha256_update(&sha_ctx, (const unsigned char *)message, strlen(message));
    mbedtls_sha256_finish(&sha_ctx, message_hash);
    mbedtls_sha256_free(&sha_ctx);

    uint8_t signature[64];
    //Firma no determinista, se utiliza un valor aleatorio para obtener la firma

    uECC_sign(privateKey, message_hash, sizeof(message_hash), signature, curve);

    // Convertir el arreglo de uint8_t en un arreglo de caracteres en hexadecimal
    char signatureHex[2 * sizeof(signature) + 1];
    for (int i = 0; i < sizeof(signature); i++) {
        sprintf(&signatureHex[2 * i], "%02x", signature[i]);
    }

    // Convertir el arreglo de caracteres en un objeto String
    String signatureString(signatureHex);

    // Convertir el arreglo de uint8_t en un arreglo de caracteres en hexadecimal
    char publicKeyHex[2 * sizeof(publicKey) + 1];
    for (int i = 0; i < sizeof(publicKey); i++) {
        sprintf(&publicKeyHex[2 * i], "%02x", publicKey[i]);
    }

    // Convertir el arreglo de caracteres en un objeto String
    String publicKeyString(publicKeyHex);

    String Address = Utils::hash(publicKey, PUBLIC_KEY_SIZE).substring(24, 64);

    Serial.println("Firma=" + signatureString  + " Clave publica=" + publicKeyString + " @Address=" + Address);

    if (uECC_verify(publicKey, message_hash, sizeof(message_hash), signature, curve)) {
        Serial.println("La firma es válida");
    } else {
         Serial.println("La firma es inválida");
    }

    if (runEvery(SENDINGTIME)) { // repeat every 1000 millis
        //sign_message(0xee8c2cf08d19374beb3aa9cda9fa53afe268dd91b5b15ae5f5645a2026f1267d,"hola", 4, firma)
        httpEth.sendData("{\"gps\": {\"latitude\": \"41.3894451\",\"longitude\": \"2.1111765\",\"altitude\": \"150.44\"},\"mac\": \"00:1B:54:11:3A:B8\",\"message\": {\"pH\": \"5\",\"tds\": \"100\"},\"provider\": \"0x21e517bf6De93b1D783fEB84ddE42F589d845CEB\"}");           
    }

}

#endif