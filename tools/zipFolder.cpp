#include "zipFolder.h"

void zipFolder::addFile(std::string filename,std::string filePath)
{
    zip_fileinfo zi={0};
    bzero(&zi.tmz_date,sizeof(zi.tmz_date));
    zipOpenNewFileInZip(handle,filePath.c_str(),&zi,NULL,0,NULL,0,NULL,Z_DEFLATED,Z_DEFAULT_COMPRESSION);
    struct stat fileState;
    stat(filename.c_str(),&fileState);
    auto *fp=fopen(filename.c_str(),"r");
    if (fp==NULL)
    {
        zipCloseFileInZip(handle);
        throw std::invalid_argument("illegal filename");
    }
    unsigned char buf[1024*1024];
    while (!feof(fp))
    {
        auto number=fread(buf,1,sizeof(buf),fp);
        zipWriteInFileInZip(handle,buf,number);
        if (ferror(fp))
        {
            zipCloseFileInZip(handle);
            throw std::runtime_error("reading aborted");
        }
    }
    zipCloseFileInZip(handle);
}
void zipFolder::compressFolder(std::string folderName,std::string relevantPath)
{
    auto *dir=opendir(folderName.c_str());
    auto *ptr=readdir(dir);
    while (ptr!=NULL)
    {
        if (ptr->d_type==4)
        {
            if (std::string _=std::string(ptr->d_name);_!="."&&_!="..")
                compressFolder(folderName+std::string(ptr->d_name)+"/",relevantPath+std::string(ptr->d_name)+"/");
        }
        else
                addFile(folderName+std::string(ptr->d_name),relevantPath+std::string(ptr->d_name));
        ptr=readdir(dir);
    }
    closedir(dir);
}
void zipFolder::save()
{
    zipClose(handle,NULL);
}