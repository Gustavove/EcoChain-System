/*
  RLP.h - RLP library for RLP functions
*/
#ifndef RLP_h
#define RLP_h

#include <stdio.h>
#include <string.h>
#include <iomanip>
#include <sstream>
#include "Trans.h"
#include <Arduino.h>

extern "C" {
    #include <stdio.h>

}

class RLP
{
  public:
    std::string encode(std::string);
	std::string encode(Trans, bool);
  	std::string encodeLength(int, int);
  	std::string intToHex(int);
  	std::string bytesToHex(std::string);
  	std::string removeHexFormatting(std::string);
  	std::string hexToRlpEncode(std::string);
  	std::string hexToBytes(std::string);
  	int char2int(char);
  	void hex2bin(const char*, char*);
  	void testPub();

  private:

};

#endif


