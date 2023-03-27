#ifndef Transaction_h
#define Transaction_h

#include <Arduino.h>
#include "Utils.h"
#include "RLP.h"

class Transaction {

    private:
        RLP rlp;
        int lol_field, l_field,nonce,gas_price;
        int gas_limit,amount,chain_id;
        String to, from, data, v,r,s;  // FF: added "from"
        String encoded, hashed, encodedSigned;
        String encode(bool);
        String hash(String);
    public:
        Transaction();
        Transaction(char* from, char* to, String data); // FF: transaction to consult value of smart contract variable
        Transaction(int nonce,int gas_price,int gas_limit, String to, int amount);
        Transaction(int nonce,int gas_price,int gas_limit, String to, int amount, String functionNme);
        String getSigned();
        String getEncoded();
        String getHashed();
        String sign(String key);
};

#endif
