#include "tcpServer.h"
#include "http.h"
#include "httpParser.h"
ltServer::ltServer(httpHandler* handler,int port,std::string ipAddress,int maxClient)
{
    signal(SIGPIPE , SIG_IGN);
    this->port=port;
    this->ipAddress=ipAddress;
    this->maxClient=maxClient;
    this->handler=handler;
}
void ltServer::addfd(int epollfd,int fd,bool oneshot)
{
    epoll_event event;
    event.data.fd=fd;
    event.events=EPOLLIN;
    if (oneshot)
        event.events|=EPOLLONESHOT;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    int old=fcntl(fd,F_GETFL);
    fcntl(fd,F_SETFL,old|O_NONBLOCK);
}
void ltServer::startForever()
{
    auto httpServer=http(handler);
    //init
    sockaddr_in servaddr,clientaddr;
    auto listenfd=socket(AF_INET,SOCK_STREAM,0);
    if (listenfd==-1)
        throw std::runtime_error("socket error");
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    inet_pton(AF_INET,ipAddress.c_str(),&servaddr.sin_addr);
    servaddr.sin_port=htons(port);
    if (bind(listenfd,(sockaddr*)&servaddr,sizeof(servaddr))==-1)
        throw std::runtime_error("bind error");

    //listen
    listen(listenfd,LISTENQ);
    epoll_event events[maxClient];
    char buf[MAXLINE];
    int epollfd=epoll_create(5);
    addfd(epollfd,listenfd,false);
    while (true)
    {
        int size=epoll_wait(epollfd,events,maxClient,-1);
        if (size<0)
        {
            throw std::runtime_error("epoll error");
        }
        std::vector<int> fds;
        for (int i=0;i<size;i++)
        {
            int* sockfd=&events[i].data.fd;
            if (*sockfd==listenfd)
            {
                socklen_t clientAddrLen=sizeof(clientaddr);
                auto connfd=accept(listenfd,(sockaddr*)&clientaddr,&clientAddrLen);
                addfd(epollfd,connfd);
            }
            else
            if (events[i].events&EPOLLIN)
            {
                auto n=read(*sockfd,buf,MAXLINE);
                if (n<=0)
                {
                    httpServer.free(*sockfd);
                    epoll_ctl(epollfd,EPOLL_CTL_DEL,*sockfd,0);
                    close(*sockfd);
                    continue;
                }
                fds.push_back(*sockfd);
                httpServer.doHttp(*sockfd,std::string(buf,n));                  
            }
        } 
        for (auto &&fd:fds)
        {
            epoll_event event;
            event.data.fd=fd;
            event.events=EPOLLIN|EPOLLONESHOT;
            epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
        }
    }
    
}
ltServer::~ltServer()
{

}