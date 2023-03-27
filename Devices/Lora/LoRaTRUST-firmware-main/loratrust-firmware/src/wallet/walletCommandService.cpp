#include "walletCommandService.h"
#include "Wallet.h"

WalletCommandService::WalletCommandService() {
    addCommand(Command("/getAddress", "Get the address of the device", 0, 1,
        [this](String args) {
        return Wallet::getInstance().getAddress();
    }));
    addCommand(Command("/getPrivateKey", "Get the private key", 1, 1,
        [this](String args) {
        return Wallet::getInstance().getPrivateKey();
    }));
    addCommand(Command("/sign", "Sign a message", 2, 1,
        [this](String args) {
        return Wallet::getInstance().sign(args);
    }));
}