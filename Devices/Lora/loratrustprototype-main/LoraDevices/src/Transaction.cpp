#include "Transaction.h"

extern "C" {
    #include <ecdsa.h>
    #include <bignum256.h>
}

Transaction::Transaction() {
    this->gas_price=20;
    this->gas_limit=21000;
    this->chain_id=1337; //local blockchain
    this->encoded = this->encodedSigned = this->hashed = "";   
}

// FF: added a transaction for consulting the state of a smart contract variable
Transaction::Transaction(char* from, char* to, String data) {
    this->to=to;
    this->from=from;
    this->data=data;
    this->chain_id=1337; //1337 local blockchain
    this->encoded = this->encodedSigned = this->hashed = "";   
}

Transaction::Transaction(int nonce,int gas_price,int gas_limit, String to, int amount) {
    this->nonce=nonce;
    this->gas_price=gas_price;
    this->gas_limit=gas_limit;
    this->to=to;
    this->amount=amount;
    this->data= "";
    this->chain_id=1; //1337 local blockchain
    this->encoded = this->encodedSigned = this->hashed = "";   
}

Transaction::Transaction(int nonce,int gas_price,int gas_limit, String to, int amount, String data) {
    this->nonce=nonce;
    this->gas_price=gas_price;
    this->gas_limit=gas_limit;
    this->to=to;
    this->amount=amount;
    this->data= data;
    this->chain_id=1; //1337 local blockchain
    this->encoded = this->encodedSigned = this->hashed = "";   
}

String Transaction::encode(bool toSign) {
    RLP rlp;
    rlp.reset();
    rlp.addInt(this->nonce);
    rlp.addLongLongInt((unsigned long long) this->gas_price*1000000000);
    rlp.addInt(this->gas_limit);
    rlp.addAddress(this->to);
    rlp.addLongLongInt((unsigned long long) this->amount*1000000000000000000);
    rlp.addString(this->data);
    if(toSign) rlp.addStringNoEncode("018080"); //eip155
    else {
        rlp.addString(this->v);
        rlp.addString(this->r);
        rlp.addString(this->s);
    }
    return rlp.getEncoded();
}

String Transaction::sign(String key) {

    this->encoded = this->encodedSigned = this->hashed = "";   

    //1-> El primer pas es codificar la transacció utilitzant RLP
    this->encoded = this->encode(true);

    //2-> El segon pas és fer el hash keccak de la transacció codificaca
    this->hashed = Utils::hash(this->encoded);

    //3-> El tercer pas és signar el hash de la transacció
    this->r = this->v = this-> s = "";
    uint8_t r[32];
    uint8_t s[64];
    uint8_t hashval[32];
    uint8_t privatekey[32];
    Utils::stringToByteArray(this->hashed, hashval);
    Utils::stringToByteArray(key, privatekey);

    ecdsaSign((BigNum256) r, (BigNum256)s, (BigNum256)hashval, (BigNum256)privatekey);

    for (int i = 31; i >= 0; i--) {
        if (r[i] <= 0xF) this->r = this->r + '0' + String(r[i], HEX);
        else this->r = this->r + String(r[i], HEX);
        if (s[i] <= 0xF) this->s = this->s + '0' + String(s[i], HEX);
        else this->s = this->s + String(s[i], HEX);
    }

    if (s[0]%2 == 0) 
        this->v = String(this->chain_id*2+35, HEX);
    else 
        this->v = String(this->chain_id*2+36, HEX); 

    this->encodedSigned = this->encode(false);
    
    return this->encodedSigned;
}

String Transaction::getSigned() {
    return this->encodedSigned;
}

String Transaction::getEncoded(){
    return this->encoded;
}   

String Transaction::getHashed() {
    return this->hashed;
}




