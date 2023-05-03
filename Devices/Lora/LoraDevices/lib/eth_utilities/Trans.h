/*
  Trans.h - Transaction library for Trans functions
*/
#ifndef Trans_h
#define Trans_h
#include<string>
class Trans
{
  public:
    std::string nonce;
    std::string gasPrice;
    std::string gasLimit;
    std::string to;
    std::string value;
    std::string data;
    std::string v;
    std::string r;
    std::string s;
  private:
};

#endif


