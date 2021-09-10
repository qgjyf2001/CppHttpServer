#include "webServer.h"
webServer::webServer(httpHandler* handler)
{
    this->handler=handler;
}
void webServer::scanDir(std::string &serveUrl,std::string &servePath,std::string path)
{
    auto *dir=opendir(path.c_str());
    auto *ptr=readdir(dir);
    while (ptr!=NULL)
    {
        if (ptr->d_type==4)
        {
            if (std::string folderName=std::string(ptr->d_name);folderName!="."&&folderName!="..")
                scanDir(serveUrl,servePath,path+folderName+"/");
        }
        else if (ptr->d_type==8)
        {
            auto fileName=path+std::string(ptr->d_name);
            auto urlName=fileName.substr(servePath.length());
            this->handler->setGetHandler(serveUrl+urlName,[fileName](httpRequestType &request,std::map<std::string,std::string>&params){
                auto response=http200BasicResponse();
                response.setFile(fileName);
                return response;
            });
        }
        ptr=readdir(dir);
    }
    closedir(dir);
}
void webServer::addHttpHook(std::string serveURL,std::string servePath)
{
    scanDir(serveURL,servePath,servePath);
    hook=new folderHook(servePath);
    hook->startHook([=](std::string fileName,bool state){
        auto urlName=fileName.substr(servePath.length());
        if (state==folderHook::hookCreated)
        {
            this->handler->setGetHandler(serveURL+urlName,[fileName](httpRequestType &request,std::map<std::string,std::string>&params){
                auto response=http200BasicResponse();
                response.setFile(fileName);
                return response;
            });
        }
        else{
            this->handler->removeGetHandler(serveURL+urlName);
        }
    });
}