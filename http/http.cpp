#include "http.h"
#include "httpBasicResponse.h"
#include "httpParser.h"

http::http(httpHandler *handler,int maxThreads)
{
    pool=new threadPool(maxThreads);
    this->handler=handler;
    sendThread=std::thread([&](){
        signal(SIGPIPE , SIG_IGN);
        while (true)
        {
            int index=0;
            while (vec.size()!=0)
            {
                auto &&[fd,sockfd,fileSize,offset]=*vec[index];
                if (offset+sendLength<fileSize)
                {
                    if (sendfile(sockfd,fd,&offset,sendLength)<sendLength)
                    {
                        delete vec[index];
                        vec.erase(std::begin(vec)+index);
                        close(fd);
                        continue;
                    }
                    index++;
                }
                else
                {
                    sendfile(sockfd,fd,&offset,fileSize-sendLength);
                    delete vec[index];
                    vec.erase(std::begin(vec)+index);
                    close(fd);
                }
                if (vec.size()==0)
                    break;
                if (index==vec.size())
                {
                    index%=vec.size();
                }
            }
            
        }
    });
}
void http::doHttp(int* sockfd,std::string httpRequest)
{
    pool->addThread([=](void *args){        
        httpRequestType request;
        try
        {
            request=httpParser::parse(httpRequest);
        }
        catch(const std::exception& e)
        {
            httpResponse resp=http400BasicResponse();
            std::string badRequest=resp.toString();
            write(*sockfd,badRequest.c_str(),badRequest.length());
            if (!resp.getConnection())
            {
                close(*sockfd);
                *sockfd=1;
            }
            return;
        }
        auto result=handler->handleRequest(request);
        auto responseText=result.toString();
        write(*sockfd,responseText.c_str(),responseText.length());
        if (result.fd!=-1)
        {
            if (result.fileSize<sendLength)
            {
                sendfile(*sockfd,result.fd,NULL,result.fileSize);
                close(result.fd);
            }
            else
                vec.push_back(new fileStruct(result.fd,*sockfd,result.fileSize,0));
        }
        if (!result.getConnection())
        {
            close(*sockfd);
            *sockfd=-1;
        }
    },&sockfd);
}
void http::waitAll()
{
    pool->waitAll();
}
http::~http()
{
    delete pool;
}