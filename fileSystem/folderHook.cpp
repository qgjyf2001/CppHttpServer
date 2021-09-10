#include "folderHook.h"
folderHook::folderHook(std::string folderPath)
{
    this->folderPath=folderPath;
}
void folderHook::addHook(int &fd,std::string folder)
{
    auto wd=inotify_add_watch(fd,folder.c_str(),IN_CREATE|IN_DELETE);
    wdMap[wd]=folder;
    invWdMap[folder]=wd;
    auto *dir=opendir(folder.c_str());
    auto *ptr=readdir(dir);
    while (ptr!=NULL)
    {
        if (ptr->d_type==4)
            if (std::string folderName=std::string(ptr->d_name);folderName!="."&&folderName!="..")
                addHook(fd,folder+folderName+"/");
        ptr=readdir(dir);
    }
    closedir(dir);
}
std::thread* folderHook::hookLoop(std::function<void(std::string fileName,bool type)> hookFunction)
{

    return new std::thread([&,hookFunction](){
#define EVENT_BUF_LEN (1024*(sizeof(inotify_event)+16))
        char buffer[EVENT_BUF_LEN];
        while (true)
        {
            int i=0;
            auto length=read(fd,buffer,EVENT_BUF_LEN);
            while (i<length)
            {
                auto *event=(inotify_event*)&buffer[i];
                if (event->len)
                {
                    if (event->mask&IN_CREATE)
                    {
                        if (event->mask&IN_ISDIR)
                        {
                            auto newFolder=wdMap[event->wd]+std::string((char*)event->name)+"/";
                            auto wd=inotify_add_watch(fd,newFolder.c_str(),IN_CREATE|IN_DELETE);
                            wdMap[wd]=newFolder;
                            invWdMap[newFolder]=wd;
                        }
                        else
                            hookFunction(wdMap[event->wd]+std::string((char*)event->name),hookCreated);
                    }
                    if (event->mask&IN_DELETE)
                    {
                        if (event->mask&IN_ISDIR)
                        {
                            auto targetFolder=wdMap[event->wd]+std::string((char*)event->name)+"/";
                            auto wd=invWdMap[targetFolder];
                            inotify_rm_watch(fd,wd);
                            wdMap.erase(wd);
                            invWdMap.erase(targetFolder);
                        }
                        else
                            hookFunction(wdMap[event->wd]+std::string((char*)event->name),hookDeleted);
                    }
                }
                i += sizeof(inotify_event) + event->len;
            }
            
        }
        
    });
}
void folderHook::startHook(std::function<void(std::string,bool type)> hookFunction)
{
    fd=inotify_init();
    addHook(fd,folderPath);
    loopThread=hookLoop(hookFunction);
}