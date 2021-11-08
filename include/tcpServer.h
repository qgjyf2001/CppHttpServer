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
#include <sys/epoll.h>

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
class ltServer
{
private:
    int port;
    std::string ipAddress;
    int maxClient;
    httpHandler *handler;
    void addfd(int epollfd,int fd,bool oneshot=false);
public:
    ltServer(httpHandler* handler,int port,std::string ipaddress="127.0.0.1",int maxClient=65535);
    void startForever();
    ~ltServer();
};
#endif