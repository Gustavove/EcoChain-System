#ifndef Utils_h
#define Utils_h

#include <Arduino.h>
#include <keccak.h>

class Utils {

public:
    static void stringToByteArray(String data, uint8_t* result);
    static String byteArrayToString(const uint8_t*, int);
    static String longDecimalToHexString(unsigned long long);
    static String intToHexString(int n);
    static int hexStringToInt(String s);
    static String hash(String);
    static String hash(uint8_t*, int);
    static String pad32Bytes(String);
};

#endif
