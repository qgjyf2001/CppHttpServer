#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <iostream>
#include <string.h>

#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

#include "httpHandler.h"

#define MAXLINE 1024
#define LISTENQ 5

class tcpServer
{
private:
    int port;
    std::string ipAddress;
    int maxClient;
    httpHandler* handler;
public:
    tcpServer(httpHandler* handler,int port,std::string ipAddress="127.0.0.1",int maxClient=65536);
    void startForever();
    ~tcpServer();
};
#endif