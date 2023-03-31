#include <Arduino.h>
#include "config.h"

#ifdef NODE
#include "string.h"
#include "HttpEth.h"
#include "Wallet.h"
#include "Utils.h"
#include "OLED.h"
#include "LoraNode.h"
#include "AES.h"

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
    httpEth.initConnection(RUT_SSID, WIFIPWD, SERVER_IP, SERVER_PORT);

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

void encrypt_message(uint8_t *key, uint8_t* text, size_t text_size, uint8_t *ciphertext){

    const int block_size = 16;

    // Calcular la cantidad de bloques necesarios
    const int num_blocks = text_size / 16;
    
    // Crear objeto AES y establecer la clave
    AES256 aes;
    aes.setKey(key, 32);

    // Array para almacenar el mensaje encriptado
    uint8_t output[text_size];

    // Encriptar cada bloque del mensaje
    for (int i = 0; i < num_blocks; i++) {
        // Obtener el bloque actual
        uint8_t block[block_size];
        int blockOffset = i * block_size;
        for (int j = 0; j < block_size; j++) {
            block[j] = text[blockOffset + j];
        }

        // Encriptar el bloque actual y agregarlo a output
        uint8_t encryptedBlock[block_size];
        aes.encryptBlock(encryptedBlock, block);
        for (int j = 0; j < block_size; j++) {
            output[blockOffset + j] = encryptedBlock[j];
        }
    }

    // Copiar el mensaje encriptado a la variable de salida
    memcpy(ciphertext, output, text_size);
}

void padding(char* temp){
    int temp_len = strlen(temp);
    int padding_len = (16 - temp_len % 16) % 16; // calcula la cantidad de ceros que se deben añadir
    for (int i = 0; i < padding_len; i++) {
       strcat(temp, "0"); // añade un cero al final de la cadena
    }
}

void loop() {

    int num = 4;
    char cadena[5];
    
    if (runEvery(SENDINGTIME)) { // repeat every 1000 millis
    uint8_t key[32] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
    
    // Definir la cadena de caracteres
    //uint8_t text[] = "{\"gps\": {\"latitude\": \"41.3894451\",\"longitude\": \"2.1111765\",\"altitude\": \"150.44\"},\"mac\":\"" + WiFi.macAddress() + "\",\"message\": {\"pH\":\""+sprintf(cadena, "%d", num)+"\",\"tds\": \"100\"},\"provider\": \"0x21e517bf6De93b1D783fEB84ddE42F589d845CEB\"}";

    char temp[208]; // Cadena temporal para construir la cadena final

    sprintf(temp, "{\"gps\": {\"latitude\": \"41.3894451\",\"longitude\": \"2.1111765\",\"altitude\": \"150.44\"},\"mac\":\"%s\",\"message\": {\"pH\":\"%d\",\"tds\": \"100\"}\"}", WiFi.macAddress().c_str(), num);

    //Añadimos 0s a la cadena spara que sea multiple de 16
    padding(temp);

    // Copiamos la cadena a la variable text para que sea uint_8
    uint8_t text[strlen(temp)];
    strncpy((char*)text, temp, strlen(temp));

    //uint8_t text[] = "Hoolaaaaaaaa0000";

    printf("El tamaño de la cadena es: %zu\n", sizeof(text));

    uint8_t ciphertextmessage[sizeof(text)];

    encrypt_message(key, text, sizeof(text), ciphertextmessage);

    // Convertir el arreglo de uint8_t en un arreglo de caracteres en hexadecimal
    Serial.print("Plaintext: ");
    for (int i = 0; i < sizeof(text); i++) {
        //Serial.print((char)text[i]);
        Serial.print(text[i], HEX);
        Serial.print(" ");
    }
    Serial.print("\n");

    Serial.print("Ciphertext: ");
    for (int i = 0; i < sizeof(ciphertextmessage); i++) {
        Serial.print(ciphertextmessage[i], HEX);
        Serial.print(" ");
    }
    Serial.print("\n");

    /*TESTING

    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    AES256 aes;
    
    aes.setKey(key, 32);

    aes.encryptBlock(ciphertext, text);
    aes.decryptBlock(decrypted, ciphertext);

    Serial.print("key: ");
    Serial.print(Utils::byteArrayToString(key, 32));
    Serial.print("\n");


    // Convertir el arreglo de uint8_t en un arreglo de caracteres en hexadecimal
    Serial.print("Plaintext: ");
    for (int i = 0; i < sizeof(text); i++) {
        Serial.print((char)text[i]);
    }
    Serial.print("\n");

    Serial.print("Ciphertext: ");
    for (int i = 0; i < sizeof(ciphertext); i++) {
        Serial.print(ciphertext[i], HEX);
        Serial.print(" ");
    }
    Serial.print("\n");

    Serial.print("Decrypted: ");
    for (int i = 0; i < sizeof(decrypted); i++) {
        Serial.print((char)decrypted[i]);
    }

    Serial.print("\n");
        
    */
        //httpEth.sendData("{\"gps\": {\"latitude\": \"41.3894451\",\"longitude\": \"2.1111765\",\"altitude\": \"150.44\"},\"mac\":\"" + WiFi.macAddress() + "\",\"message\": {\"pH\":\""+sprintf(cadena, "%d", num)+"\",\"tds\": \"100\"},\"provider\": \"0x21e517bf6De93b1D783fEB84ddE42F589d845CEB\"}");
    
    }

    num++;
}

#endif