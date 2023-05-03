#include "Utils.h"

void Utils::stringToByteArray(String data, uint8_t* result) {
    uint8_t buff;
    char aux;
    int j = 0;
    for (int i = 0; i < data.length(); i = i + 2) {
        buff = 0;
        aux = data[i];
        if (aux >= '0' and aux <= '9') buff = (buff + (aux - '0')) * 16;
        else if (aux >= 'A' and aux <= 'Z') buff = (buff + (aux - 'A' + 10)) * 16;
        else if (aux >= 'a' and aux <= 'z') buff = (buff + (aux - 'a' + 10)) * 16;
        aux = data[i + 1];
        if (aux >= '0' and aux <= '9') buff = (buff + (aux - '0'));
        else if (aux >= 'A' and aux <= 'Z') buff = (buff + (aux - 'A' + 10));
        else if (aux >= 'a' and aux <= 'z') buff = (buff + (aux - 'a' + 10));
        result[j] = buff;
        j++;
    }
}

String Utils::byteArrayToString(const uint8_t* data, int data_size) {
    String result = "";
    for (int i = 0; i < data_size; i++) {
        String aux = String(data[i], HEX);
        if (aux.length() == 1) aux = "0" + aux;
        result = result + aux;
    }
    return result;
}

String Utils::longDecimalToHexString(unsigned long long n) {
    String hex_value = "";
    char buf[50];
    sprintf(buf, "%llu", n);
    while (n > 0) {
        int temp = 0;
        int  divisor = 16;
        char aux;
        temp = n % divisor;
        if (temp < 10) {
            aux = temp + '0';
            hex_value = aux + hex_value;
        }
        else {
            aux = (temp - 10) + 'A';
            hex_value = aux + hex_value;
        }

        n = n / divisor;
    }
    if (hex_value.length() % 2 > 0) hex_value = "0" + hex_value;
    return hex_value;
}


String Utils::hash(String data) {
    int datasize = data.length() / 2;
    uint8_t result[datasize];
    Keccak keccak;
    keccak.reset();
    Utils::stringToByteArray(data, result);
    keccak.add(result, datasize);
    return keccak.getHash().c_str();
}

String Utils::hash(uint8_t* data, int size) {
    Keccak keccak;
    keccak.reset();
    keccak.add(data, size);
    String myHash = keccak.getHash().c_str();
    return myHash;
}

String Utils::pad32Bytes(String data) {
    String s = String(data);
    while (s.length() < 64) { s = "0" + s; }
    return s;
}

String Utils::intToHexString(int n) {
    String hex_value = "";
    char buf[50];
    sprintf(buf, "%llu", n);
    while (n > 0) {
        int temp = 0;
        int  divisor = 16;
        char aux;
        temp = n % divisor;
        if (temp < 10) {
            aux = temp + '0';
            hex_value = aux + hex_value;
        }
        else {
            aux = (temp - 10) + 'A';
            hex_value = aux + hex_value;
        }

        n = n / divisor;
    }
    if (hex_value.length() % 2 > 0) hex_value = "0" + hex_value;
    return hex_value;
}

int Utils::hexStringToInt(String s) {
    int n = 0;
    for (int i = 0; i < s.length(); i++) {
        n = n * 16;
        char c = s[i];
        if (c >= '0' and c <= '9') n = n + (c - '0');
        else if (c >= 'A' and c <= 'Z') n = n + (c - 'A' + 10);
        else if (c >= 'a' and c <= 'z') n = n + (c - 'a' + 10);
    }
    return n;
}