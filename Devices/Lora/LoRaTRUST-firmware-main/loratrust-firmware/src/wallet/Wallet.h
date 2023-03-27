/**
  Project: LoRa Trust firmware
  File name: Wallet.h
  File type: Class declarations (header)
  Purpose: Wallet software. Key and data signatures management.
  Detail: This class aims to provide the hardware with the functions necessary to create
  a private key (32 random bytes vector) and its corresponding ECDSA public key. The private
  key will be used to sign data. The public key will be used to identify the device and will
  be printed in the screen. Signature functions are provided in this class too.

  @author Eloi Cruz Harillo
  @version 1.0 10/10/2022
*/

#ifndef Wallet_h
#define Wallet_h

extern "C" {
#include <ecdsa.h>
#include <bignum256.h>
}

#include <Arduino.h>
#include <uECC.h>
#include "configuration/configService.h"
#include "helpers/helper.h"

#include "message/messageManager.h"

#include "message/messageService.h"

#include "walletCommandService.h"

#define PRIVATE_KEY_SIZE 32
#define PUBLIC_KEY_SIZE 64
#define ADDRESS_SIZE 20

class Wallet: public MessageService {
public:
    static Wallet& getInstance() {
        static Wallet instance;
        return instance;
    }

    void begin();
    static int RNG(uint8_t* dest, unsigned size);
    String getAddress();
    String sign(String data);
    String getPrivateKey();

private:
    WalletCommandService* walletCommandService = new WalletCommandService();

    Wallet(): MessageService(appPort::WalletApp, String("Wallet")) {
        commandService = walletCommandService;
    };

    void generateKeyAddress(bool reset);

    uint8_t privateKey[PRIVATE_KEY_SIZE];
    uint8_t publicKey[PUBLIC_KEY_SIZE];
    String address;
};


#endif