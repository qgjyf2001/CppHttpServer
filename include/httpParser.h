#ifndef HTTPPARSER_H
#define HTTPPARSER_H
#include <iostream>
#include <map>
using httpRequestType=std::map<std::string,std::string>;
class httpParser
{
private:
    static std::string parseToken(std::string& str,int &pos,int &length,char token);
public:
    httpParser()=default;
    static httpRequestType parse(std::string httpRequest);
    ~httpParser()=default;
};
#endif