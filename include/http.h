#ifndef HTTP_H
#define HTTP_H
#include <iostream>
#include <condition_variable>
#include <unistd.h>
#include "httpHandler.h"
#include "httpResponse.h"
#include "threadPool.h"
#include "safeVector.h"

#include <sys/sendfile.h>
#include <signal.h>
#include <map>
class http
{
private:
    struct fileStruct
    {
        fileStruct(int fd,int sockfd,long int fileLength,long int offset):fd(fd),sockfd(sockfd),fileLength(fileLength),offset(offset)
        {

        }
        int fd;
        int sockfd;
        long int fileLength;
        long int offset;
    };
    /* data */
    int sendLength=1024*1024*10;
    threadPool* pool;
    httpHandler* handler;
    std::thread sendThread;
    safeVector<fileStruct*> vec;
    std::map<int,std::pair<std::string,int>> uncompleted;
    std::mutex mutex;
    std::condition_variable consumer;
public:
    http(httpHandler* handler,int maxThreads=4);
    void doHttp(int* sockfd,std::string httpRequest,std::function<void(int*)> handleClose);
    void free(int sockfd);
    void waitAll();
    ~http();
};
#endif
