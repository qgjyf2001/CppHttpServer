#include "threadPool.h"
#include "tcpServer.h"
#include "httpParser.h"
#include "webServer.h"
#include "jsonParser.h"
#include "safeVector.h"

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

int main()
{
   /* auto* tp=new threadPool(4);
    int a=0;
    for (int i=0;i<100000;i++)
        tp->addThread(
            [i,&a](void *arg){
                //printf("%dth work done!\n",i);
                a++;
                }
            ,nullptr);
    tp->waitAll();
    delete tp;
    printf("%d\n",a);*/
    auto *handler=new httpHandler();
    tcpServer *server=new tcpServer(handler,8080,"0.0.0.0");
    auto *web=new webServer(handler);
    web->addHttpHook();
    handler->setGetHandler("/file",[](httpRequestType &request,std::map<std::string,std::string>&params){
        auto response=http200BasicResponse();
        response.setFile("./"+params["file"]);
        return response;
    });
    handler->setGetHandler("/api/dir",[](httpRequestType &request,std::map<std::string,std::string> &params)->httpResponse{
        auto response=http200BasicResponse();
        auto dirPath="./"+params["dir"];
        auto *dir=opendir(dirPath.c_str());
        if (dir==NULL)
            return http404BasicResponse();
        dirent *ptr=readdir(dir);
        std::string arr="[]";
        JsonParser jsonArray(&arr,JsonParser::ARRAY);
        try
        {    
            while (ptr!=NULL){
                JsonParser* json=new JsonParser;
                json->set("name",new JsonParser(new std::string(ptr->d_name),JsonParser::STRING));
                json->set("type",new JsonParser(ptr->d_type));
                jsonArray.add(json);
                ptr=readdir(dir);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        closedir(dir);
        response.setJson((std::string)jsonArray);
        return response;

    });
    handler->setPostHandler("/test",[](httpRequestType &request,httpPostRequestContent& content){
        auto response=http200BasicResponse();
        JsonParser json;
        if (content.type==httpPostRequestContent::JSON)
        {
            json.set("type",new JsonParser(new std::string("json"),JsonParser::STRING));
            json.set("data",new JsonParser(new std::string((*content.json)["data"]),JsonParser::STRING));
        }
        if (content.type==httpPostRequestContent::FORM)
        {
            json.set("type",new JsonParser(new std::string("form"),JsonParser::STRING));
            json.set("data",new JsonParser(new std::string(content.form["data"]),JsonParser::STRING));
        }
        response.setJson(json);
        return response;
    });
    server->startForever();
    return 0;
}