#include <Arduino.h>
#include "config.h"

#ifdef NODE
#include "string.h"
#include "HttpEth.h"
#include "Wallet.h"
#include "Utils.h"
#include "OLED.h"
#include "LoraNode.h"
#include <uECC.h>
#include <SHA256.h>

HttpEth httpEth;
LoraNode lora;
OLED display;


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

// Estrucutura y funciones de contexto para firma determinista
typedef struct SHA256_HashContext {
    uECC_HashContext uECC;
} SHA256_HashContext;

void init_SHA256(const uECC_HashContext *base) {
    SHA256_HashContext *context = (SHA256_HashContext *)base;
}

void update_SHA256(const uECC_HashContext *base,
                   const uint8_t *message,
                   unsigned message_size) {
    SHA256_HashContext *context = (SHA256_HashContext *)base;
    SHA256 sha256;
    sha256.update(message, message_size);
}

void finish_SHA256(const uECC_HashContext *base, uint8_t *hash_result) {
    SHA256_HashContext *context = (SHA256_HashContext *)base;
    SHA256 sha256;
    sha256.finalize(hash_result, 32);
}


void loop() {

    uint8_t privateKey[32] = {0xee,0x8c,0x2c,0xf0, 0x8d, 0x19, 0x37, 0x4b, 0xeb, 0x3a, 0xa9, 0xcd, 0xa9, 0xfa, 0x53, 0xaf, 0xe2, 0x68,0xdd, 0x91, 0xb5, 0xb1, 0x5a, 0xe5, 0xf5, 0x64, 0x5a, 0x20, 0x26, 0xf1, 0x26, 0x7d};
    uint8_t publicKey[64];
    const struct uECC_Curve_t* curve = uECC_secp256k1();
    uECC_compute_public_key(privateKey, publicKey, curve);
    char message[] = "Holaa";

    SHA256 sha256;
    unsigned char message_hash[32];
    sha256.update((const uint8_t*)message, strlen(message));
    sha256.finalize(message_hash, sizeof(message_hash));

    uint8_t signature[64];


    // Se genera la firma determinista
    uint8_t tmp[32 + 32 + 64];
    SHA256_HashContext ctx = {{&init_SHA256, &update_SHA256, &finish_SHA256, 64, 32, tmp}};
    uECC_sign_deterministic(privateKey, message_hash, 32, &ctx.uECC, signature, curve);

    /*Firma no determinista, se utiliza un valor aleatorio para obtener la firma
    SHA256 sha256;
    unsigned char message_hash[32];
    sha256.update((const uint8_t*)message, strlen(message));
    sha256.finalize(message_hash, sizeof(message_hash));
    uECC_sign(privateKey, message_hash, sizeof(message_hash), signature, curve);
    */

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

        httpEth.sendData("{\"gps\": {\"latitude\": \"41.3894451\",\"longitude\": \"2.1111765\",\"altitude\": \"150.44\"},\"mac\": \"00:1B:54:11:3A:B8\",\"message\": {\"pH\": \"5\",\"tds\": \"100\"},\"provider\": \"0x21e517bf6De93b1D783fEB84ddE42F589d845CEB\"}");
    }

}

#endif