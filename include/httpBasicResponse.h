#ifndef HTTPBASICRESPONSE_H
#define HTTPBASICRESPONSE_H
#include "httpResponse.h"
const std::string default404Html=R"(<html>
    <body>
        <div>cannot find Page</div>
    </body>
</html>)";
class http404BasicResponse:public httpResponse{
public:
    http404BasicResponse(std::string text=default404Html,std::string description="not found"):httpResponse()
    {
        this->setCode(404);
        this->setDescription(description);
        this->setText(text);
    }
    ~http404BasicResponse()=default;
};
class http400BasicResponse:public httpResponse{
    public:
    http400BasicResponse(std::string description="Bad Request"):httpResponse()
    {
        this->setCode(400);
        this->setDescription(description);      
    }
};
class http200BasicResponse:public httpResponse{
public:
    http200BasicResponse(std::string description="OK"):httpResponse()
    {
        this->setCode(200);
        this->setDescription(description);
    }   
};
#endif