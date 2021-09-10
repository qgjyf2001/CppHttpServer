#ifndef FOLDERHOOK_H
#define FOLDERHOOK_H
#include <iostream>
#include <map>
#include <functional>
#include <thread>

#include <dirent.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/types.h>
class folderHook
{
private:
    std::string folderPath;
    std::map<int,std::string> wdMap;
    std::map<std::string,int> invWdMap;
    void addHook(int &fd,std::string folder);
    std::thread* hookLoop(std::function<void(std::string fileName,bool type)> hookFunction);
    std::thread* loopThread;
    int fd;
public:
enum{
    hookCreated,
    hookDeleted
}hookState;
    folderHook(std::string folderPath);
    void startHook(std::function<void(std::string fileName,bool type)> hookFunction);
    ~folderHook()=default;    
};
#endif