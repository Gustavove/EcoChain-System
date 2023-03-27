#ifndef Wallet_h
#define Wallet_h

#include <Arduino.h>
#include <uECC.h>
#include <Preferences.h>
#include "Utils.h"

#define PRIVATE_KEY_SIZE 32
#define PUBLIC_KEY_SIZE 64
#define ADDRESS_SIZE 20

class Wallet {

    uint8_t privateKey[PRIVATE_KEY_SIZE];
    uint8_t publicKey[PUBLIC_KEY_SIZE];
    String address;
    Preferences preferences;

public:
    Wallet();
    void begin();
    String getPrivateKey();
    String getAddress();
    void generateKeyAddress(bool reset);
    static int RNG(uint8_t* dest, unsigned size);
};

extern Wallet wallet;

#endif