#ifndef RLP_H
#define RLP_H

#include <Arduino.h>
#include "Utils.h"


class RLP {

    private:
        String encodedData;
        String encodeLength(String, bool);
    public:
        RLP();
        void reset();
        void addString(String);
        void addStringNoEncode(String);
        void addInt(int);
        void addLongLongInt(unsigned long long);
        void addAddress(String);
        String getEncoded();

};

#endif