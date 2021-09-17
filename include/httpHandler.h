#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H
#include "httpParser.h"
#include "functional"
#include "httpBasicResponse.h"
#include <jsonParser.h>
struct httpPostRequestContent
{
    enum Type{
      JSON,
      FORM,
      OTHER
    };
    Type type;
    std::map<std::string,std::string> form;
    JsonParser* json=NULL;
    httpPostRequestContent(Type type=OTHER):type(type)
    {

    }
};
class httpHandler
{
private:
    /* data */
    std::map<std::string,std::string> getParams(std::string& params,int offset);
    std::map<std::string,std::function<httpResponse(httpRequestType&,std::map<std::string,std::string>&)>> getHandlers;
    std::map<std::string,std::function<httpResponse(httpRequestType&,httpPostRequestContent&,std::map<std::string,std::string>&)>> postHandlers; 
public:
    httpHandler(/* args */)=default;
    httpResponse handleRequest(httpRequestType request);
    void setGetHandler(std::string url,std::function<httpResponse(httpRequestType&,std::map<std::string,std::string>&)> handler)
    {
        getHandlers[url]=handler;
    }
    void removeGetHandler(std::string url)
    {
        getHandlers.erase(url);
    }
    void setPostHandler(std::string url,std::function<httpResponse(httpRequestType&,httpPostRequestContent&,std::map<std::string,std::string>&)> handler)
    {
        postHandlers[url]=handler;
    }
    void removePostHandler(std::string url)
    {
        postHandlers.erase(url);
    }
    ~httpHandler()=default;
};
#endif