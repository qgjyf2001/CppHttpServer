#include "httpHandler.h"
std::map<std::string,std::string> httpHandler::getParams(std::string &params,int offset)
{
    std::map<std::string,std::string> requestParam;
    int len=params.length();
    for (int forwardPtr=len-1,backwardPtr=len-1,midPtr=-1;forwardPtr>=offset;forwardPtr--)
        {
            if (params[forwardPtr]=='=')
                midPtr=forwardPtr;
            else if (params[forwardPtr]=='&'||params[forwardPtr]=='?')
            {
                if (midPtr==-1)
                    requestParam[params.substr(forwardPtr+1,backwardPtr-forwardPtr)]="";
                else
                    requestParam[params.substr(forwardPtr+1,midPtr-forwardPtr-1)]=params.substr(midPtr+1,backwardPtr-midPtr);
                backwardPtr=forwardPtr-1;
                midPtr=-1;
            }
        }
    return requestParam;
}
httpResponse httpHandler::handleRequest(httpRequestType request)
{
    httpResponse response;        
    if (auto &method=request["method"];method=="GET")
    {
        auto& url=request["url"];
        int i;
        auto len=url.length();
        std::map<std::string,std::string> requestParam;
        for (i=len-1;i>=0;i--)
            if (url[i]=='?')
                break;
        if (i>0)
        {
            requestParam=getParams(url,i);
            url=url.substr(0,i);
        }
        auto it=getHandlers.find(url);
        if (it==getHandlers.end())
        {
            response=http404BasicResponse();
        }
        else
        {
            return getHandlers[url](request,requestParam);
        }
        return response;
    }
    else if (method=="POST")
    {
        auto &url=request["url"];
        auto it=postHandlers.find(url);
        auto &type=request["Content-Type"];
        auto content=httpPostRequestContent();
        if (it==postHandlers.end())
        {
            response=http404BasicResponse();
        }        
        else
        {
            if (type.find("application/x-www-form-urlencoded")!=std::string::npos)
            {
                content.type=httpPostRequestContent::FORM; 
                std::string params=std::string("?")+request["text"];
                content.form=getParams(params,0);
            }
            if (type.find("application/json")!=std::string::npos)
            {
                
                try
                {
                    content.type=httpPostRequestContent::JSON;
                    content.json=new JsonParser(&request["text"]);
                }
                catch(const std::exception& e)
                {
                    return http400BasicResponse();
                }
            }
            response=postHandlers[url](request,content);
            if (content.json!=NULL)
                delete content.json;
        }
        return response;
    }
    return http400BasicResponse();
}