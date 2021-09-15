#ifndef ZIPFOLDER_H
#define ZIPFOLDER_H
#include <iostream>

#include <strings.h>
#include <minizip/zip.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
class zipFolder
{
private:
    zipFile handle;
    void addFile(std::string filename,std::string filePath);
public:
    zipFolder(std::string zipFileName)
    {
        handle=zipOpen(zipFileName.c_str(),APPEND_STATUS_CREATE);
    }
    void compressFolder(std::string folderName,std::string relevantPath="");
    void save();
    ~zipFolder()=default;
};


#endif