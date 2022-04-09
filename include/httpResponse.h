#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <iostream>
#include <map>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class httpResponse
{
private:
    /* data */
    int code;
    std::string httpVersion="HTTP/1.1";
    std::string description;
    std::map<std::string,std::string> headers;
    std::string text;
    bool connection=false;    
    std::map<std::string,std::string> fileMapping={
        {"js","text/javascript"},
        {"css","text/css"},
        {"htm","text/html"},
        {"html","text/html"},
        {"gif","image/gif"},
        {"jpg","image/jpeg"},
        {"jpeg","image/jpeg"},
        {"png","image/png"},
        {"txt","text/plain"},
        {"svg","text/xml"}};
public:    
    int fd=-1;
    unsigned long fileSize;
    httpResponse(/* args */){
        headers["Server"]="MyCppServer";
        setConnection(connection);
    }
    void setConnection(bool connection)
    {
        this->connection=connection;
        if (connection)
            headers["Connection"]="keep-alive";
        else
            headers["Connection"]="close";
    }
    bool getConnection()
    {
        return connection;
    }
    void setCode(int code)
    {
        this->code=code;
    }
    void setHttpVersion(std::string version)
    {
        httpVersion=version;
    }
    void setDescription(std::string description)
    {
        this->description=description;
    }
    void setHeaders(std::string key,std::string value)
    {
        headers[key]=value;
    }
    void setServer(std::string value)
    {
        headers["Server"]=value;
    }
    void setContentType(std::string value)
    {
        headers["Content-Type"]=value;
    }
    void setText(std::string value)
    {
        text=value;
        this->setContentType("text/html");
        headers["Content-Length"]=std::to_string(text.length());
    }
    void setJson(std::string value)
    {
        text=value;
        this->setContentType("application/json");
        headers["Content-Length"]=std::to_string(text.length());
    }
    void setFileContent(std::string fileExtention,int length)
    {
        if (auto it=fileMapping.find(fileExtention);it==fileMapping.end())
            this->setContentType(fileMapping["txt"]);
        else
            this->setContentType(fileMapping[fileExtention]);
        headers["Content-Length"]=std::to_string(length);
    }
    void setFile(std::string filePath)
    {
        try
        {
            struct stat state;
            stat(filePath.c_str(),&state);
            fileSize=state.st_size;
            fd=open(filePath.c_str(),O_RDONLY);
            int pos=-1;
            for (int i=0;i<filePath.length();i++)
            if (filePath[i]=='.')
                pos=i;
            if (pos!=-1)
                setFileContent(filePath.substr(pos+1,filePath.length()-pos-1),fileSize);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
    virtual std::string toString()
    {
        std::string result=httpVersion+" "+std::to_string(code)+" "+description+"\r\n";
        for (auto &&key:headers)
            result+=key.first+":"+key.second+"\r\n";
        result+="\r\n";
        result+=text;
        return result;
    }
    ~httpResponse()=default;
};
#endif
