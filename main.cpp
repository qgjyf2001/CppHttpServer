#include "threadPool.h"
#include "tcpServer.h"
#include "httpParser.h"
#include "webServer.h"
#include "jsonParser.h"
#include "safeVector.h"

#include "tools/zipFolder.h"

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
    web->addHttpHook("/","./app/build/");
    handler->setGetHandler("/file",[](httpRequestType &request,std::map<std::string,std::string>&params){
        auto response=http200BasicResponse();
        response.setFile("./"+params["file"]);
        return response;
    });
    handler->setGetHandler("/folder",[](httpRequestType &request,std::map<std::string,std::string>& params){
        auto response=http200BasicResponse();
        std::string fileName="./tmp/";
        for (int i=0;i<10;i++)
            fileName+='a'+rand()%26;
        fileName+=".zip";
        auto zipContext=zipFolder(fileName);
        zipContext.compressFolder("./"+params["folder"]);
        zipContext.save();
        response.setFile(fileName);
        remove(fileName.c_str());
        return response;
    });
    handler->setGetHandler("/api/remove",[](httpRequestType &request,std::map<std::string,std::string> &params){
        auto response=http200BasicResponse();
        JsonParser json;
        json.set("status",remove(("./"+params["file"]).c_str())?"failed":"success");
        response.setJson(json);
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
                json->set("name",ptr->d_name);
                json->set("type",ptr->d_type);
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
            json.set("type","json");
            json.set("data",(*content.json)["data"]);
        }
        if (content.type==httpPostRequestContent::FORM)
        {
            json.set("type","form");
            json.set("data",std::string(content.form["data"]));
        }
        response.setJson(json);
        return response;
    });
    server->startForever();
    return 0;
}