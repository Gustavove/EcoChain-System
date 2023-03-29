#include "Wallet.h"

Wallet::Wallet() {
}

void Wallet::begin() {
  uECC_set_rng(&RNG);
  randomSeed(analogRead(0));

  this->address = "";
  this->preferences.begin("data", false);

  this->generateKeyAddress(true);
}

String Wallet::getPrivateKey() {
  String key;
  for (int i = 0; i < PRIVATE_KEY_SIZE; i++) {
    String aux = String(this->privateKey[i], HEX);
    if (aux.length() == 1) aux = "0" + aux;
    key = key + aux;
  }
  return key;
}

String Wallet::getAddress() {
  return this->address;
}

int Wallet::RNG(uint8_t* dest, unsigned size) {
  while (size) {
    uint8_t aux = (uint8_t) random(0, 255);
    *dest = aux;
    ++dest;
    --size;
  }
  return 1;
}

void Wallet::generateKeyAddress(bool reset) {

  //Si la EEPROM ja te adreça guardada la llegim i sino la generem
  //Comprovem si la placa ja te  una adreça guardada (els valors 100 i 1 són arbitraris

  if (!reset && preferences.getBool("stored")) {
    Serial.println("Adress actually created. Reading from persistence layer...");
    Utils::stringToByteArray(preferences.getString("privateKey"), this->privateKey);
    this->address = preferences.getString("address");
  }
  else {
    Serial.println("No address saved or reset mode, generating new one...");
    const struct uECC_Curve_t* curve = uECC_secp256k1();
    if (uECC_make_key(this->publicKey, this->privateKey, curve)) Serial.println("success");
    else Serial.println("error");

    this->address = Utils::hash(this->publicKey, PUBLIC_KEY_SIZE).substring(24, 64);

    //Save to EEPROM
    //escribim dos valors arbitraris per registrar que hi ha dades valides

    preferences.putBool("stored", true);
    preferences.putString("privateKey", Utils::byteArrayToString(this->privateKey, PRIVATE_KEY_SIZE));
    preferences.putString("address", this->address);
  }
}

Wallet wallet = Wallet();