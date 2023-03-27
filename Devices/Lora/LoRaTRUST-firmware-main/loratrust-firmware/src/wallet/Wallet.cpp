/**
  Project: LoRa Trust firmware
  File name: Wallet.cpp
  File type: Class definitions (implementation)
  Purpose: Wallet software. Key and data signatures management.
  Detail: Definition of the functions declared int he file Wallet.h

  @author Eloi Cruz Harillo
  @version 1.0 10/10/2022
*/

#include "Wallet.h"

/**
  Initialize class components.
*/
void Wallet::begin() {
  uECC_set_rng(&RNG);
  randomSeed(analogRead(A0));

  generateKeyAddress(false);
}

/**
  Device private key getter.

  @return key -> private key
*/
String Wallet::getPrivateKey() {
  String key;
  for (int i = 0; i < PRIVATE_KEY_SIZE; i++) {
    String aux = String(this->privateKey[i], HEX);
    if (aux.length() == 1) aux = "0" + aux;
    key = key + aux;
  }
  return key;
}

/**
  Device address getter.

  @return address
*/
String Wallet::getAddress() {
  return this->address;
}

/**
  Random seed generator.

  @param dest pointer to desr tandom number
  @param size size of poiner data
  @return 1 if success
*/
int Wallet::RNG(uint8_t* dest, unsigned size) {
  while (size) {
    uint8_t aux = (uint8_t) random(0, 255);
    *dest = aux;
    ++dest;
    --size;
  }
  // Hashing data here could be a good idea to improve random data
  return 1;
}

/**
  Generate or recover from EEPROM private and public keys and public address.

  @param reset true->generate key thought it is in EEPROM already. false-> check
  in EEPROM memory if not null recover, if null generate it.

*/
void Wallet::generateKeyAddress(bool reset) {
  ConfigService& configService = ConfigService::getInstance();

  //Get the Key if it is in the device, else generate a new key
  String privateKey = configService.getConfig("privateKey", "");
  String address = configService.getConfig("address", "");

  bool isKeyGenerated = privateKey != "" && address != "";

  if (!reset && isKeyGenerated) {
    Serial.println("[I] ---------- M: Address actually created. Reading from persistence layer...");
    Helper::stringToByteArray(privateKey, this->privateKey);
    this->address = address;
  }
  else {
    Serial.println("[I] ---------- M: No address saved or reset mode, generating new one...");
    const struct uECC_Curve_t* curve = uECC_secp256k1();
    if (uECC_make_key(this->publicKey, this->privateKey, curve)) Serial.println("success");
    else Serial.println("error");

    this->address = Helper::hash(this->publicKey, PUBLIC_KEY_SIZE).substring(24, 64);

    //Save to EEPROM

    configService.setConfig("privateKey", Helper::uint8ArrayToString(this->privateKey, PRIVATE_KEY_SIZE));
    configService.setConfig("address", this->address);
  }
}

/**
  ECDSA sign atring data using private key

  @param data data string to be signed

*/
String Wallet::sign(String data) {

  //1-> El primer pas és fer el hash keccak de les dades
  //El hash te sempre 64 bytes i s'utilitza per signar.
  String hashedData = Helper::hash(data);

  //2-> El segon pas és signar el hash de la transacció
  String r, s, v;
  r = v = s = "";
  uint8_t _r[32];
  uint8_t _s[64];
  uint8_t hashval[32];
  uint8_t privatekey[32];
  // memcpy(hashval, hashedData.c_str(), 32);
  // memcpy(privatekey, this->privateKey, 32);
  Helper::stringToByteArray(hashedData.substring(0, 32), hashval);
  Helper::stringToByteArray(getPrivateKey(), privatekey);

  ecdsaSign((BigNum256) _r, (BigNum256) _s, (BigNum256) hashval, (BigNum256) privatekey);

  // RLP rlp;

  // r = "0x" + rlp.bytesToHex(std::string(Helper::uintToCharArr(_r, 32), 32));
  // s = "0x" + rlp.bytesToHex(std::string(Helper::uintToCharArr(_s, 32), 32));

  for (int i = 31; i >= 0; i--) {
    if (_r[i] <= 0xF) r = r + '0' + String(_r[i], HEX);
    else r = r + String(_r[i], HEX);
    if (_s[i] <= 0xF) s = s + '0' + String(_s[i], HEX);
    else s = s + String(_s[i], HEX);
  }

  String rs = r + s;
  return rs.substring(0, 64);
}
