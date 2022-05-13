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
            std::unique_lock<std::mutex> lck(mutex);
            consumer.wait(lck);
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
void http::free(int sockfd)
{
    if (uncompleted.find(sockfd)!=uncompleted.end())
        uncompleted.erase(sockfd);
    if (futureMap.find(sockfd)!=futureMap.end())
        futureMap.erase(sockfd);
}
void http::doHttp(int sockfd,std::string httpRequest)
{
    auto res=pool->addThread([=](void *args)->httpReturnType{    
        httpReturnType httpResult; 
        signal(SIGPIPE , SIG_IGN);
        httpRequestType request;
        if (uncompleted.find(sockfd)!=uncompleted.end())
        {
        if (auto &now=uncompleted[sockfd];now.second!=0)
        {
               now.second-=httpRequest.length();
               if (now.second>0)
               {
                   now.first+=httpRequest;
                   return std::make_pair(false,"");
               }
                else
                {
                    now.second=0;
                    try
                    {
                        request=httpParser::parse(now.first+httpRequest);
                    }
                    catch(const std::exception& e)
                    {
                        httpResponse resp=http400BasicResponse();
                        std::string badRequest=resp.toString();
                        uncompleted.erase(sockfd);
                        return std::make_pair(!resp.getConnection(),std::move(badRequest));
                    }
                    uncompleted.erase(sockfd);
                }
        }
    }
        else 
        {
            try
            {
                request=httpParser::parse(httpRequest);    
                if (auto len1=std::atoi(request["Content-Length"].c_str()),len2=(int)request["text"].length();len1>len2)
                {
                    uncompleted[sockfd]=std::make_pair(httpRequest,len1-len2);
                    return std::make_pair(false,"");
                }
            }
            catch(const std::exception& e)
            {
                httpResponse resp=http400BasicResponse();
                std::string badRequest=resp.toString();
                httpResult.first=!resp.getConnection();
                httpResult.second=std::move(resp.toString());
                return httpResult;
            }
        }
        auto result=handler->handleRequest(request);
        auto responseText=result.toString();
        if (result.fd!=-1)
        {
            write(sockfd,responseText.c_str(),responseText.length());
            if (result.fileSize<sendLength)
            {
                sendfile(sockfd,result.fd,NULL,result.fileSize);
                close(result.fd);
            }
            else
            {
                vec.push_back(new fileStruct(result.fd,sockfd,result.fileSize,0));
                if (vec.size()==1)
                {
                    std::unique_lock<std::mutex> lck(mutex);
                    consumer.notify_one();
                }
            }
            return std::make_pair(false,"");
        }
        httpResult.first=!result.getConnection();
        httpResult.second=std::move(responseText);
        return httpResult;
    },&sockfd);
    futureMap[sockfd]=std::move(res);
}
http::httpReturnType http::getResult(int sockfd,bool& result) {
    if (futureMap.find(sockfd)==futureMap.end()) {
        result=false;  
        return http::httpReturnType();
    }
    result=true;
    auto &future=futureMap[sockfd];
    if (future.wait_for(std::chrono::duration(std::chrono::seconds(0)))==std::future_status::timeout) {
        return http::httpReturnType(false,"");
    }
    auto httpResult=future.get();
    futureMap.erase(sockfd);
    return httpResult;
}
http::~http()
{
    delete pool;
}