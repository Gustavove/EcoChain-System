#include "RLP.h"

RLP::RLP() {
    this->encodedData = "";
}

void RLP::reset() {
    this->encodedData = "";
}

String RLP::encodeLength(String data, bool list) {
    int mida;
    char buff[10];
    if (data.length() % 2 == 0) mida = data.length() / 2;
    else mida = (data.length() / 2) + 1;

    if (mida == 1) {    
        data.toCharArray(buff, (unsigned int) 10);
    }

    if (!list) {
        if (data.equals("00"))
            return "80";
        else if (mida == 1 && buff[0] < 128)
            return data;

        else if (mida < 55) {
            int size = 128 + mida;
            String sizeString = String(size, HEX);
            if (sizeString.length() % 2 == 1) sizeString = "0" + sizeString;
            return sizeString + data;
        }
        else {
            int size = mida;
            String sizeString = String(size, HEX);
            if (sizeString.length() % 2 == 1) sizeString = "0" + sizeString;
            int lol = 183 + (sizeString.length() / 2);
            String lolString = String(lol, HEX);
            if (lolString.length() % 2 == 1) lolString = "0" + lolString;
            return lolString + sizeString + data;
        }
    }
    else {
        if (mida < 55) {
            int size = 192 + mida;
            String sizeString = String(size, HEX);
            if (sizeString.length() % 2 == 1) sizeString = "0" + sizeString;
            return sizeString + data;
        }
        else { ///AQUESTA POSSIBILITAT S'HA DE PROGRAMAR CORRECTAMENT!!
            int size = mida;
            String sizeString = String(size, HEX);
            if (sizeString.length() % 2 == 1) sizeString = "0" + sizeString;
            int lol = 247 + (sizeString.length() / 2);
            String lolString = String(lol, HEX);
            if (lolString.length() % 2 == 1) lolString = "0" + lolString;
            return lolString + sizeString + data;
        }
    }
}

void RLP::addAddress(String data) {
    this->encodedData = this->encodedData + "94" + data;
}

void RLP::addString(String data) {
    this->encodedData = this->encodedData + encodeLength(data, false);
}

void RLP::addStringNoEncode(String data) {
    this->encodedData = this->encodedData + data;
}

void RLP::addInt(int data) {
    String dat = String(data, HEX);
    if (dat.length() % 2 == 1) dat = "0" + dat;
    addString(dat);
}


void RLP::addLongLongInt(unsigned long long data) {
    String dat = Utils::longDecimalToHexString(data);
    addString(dat);
}

String RLP::getEncoded() {
    return encodeLength(this->encodedData, true);
}