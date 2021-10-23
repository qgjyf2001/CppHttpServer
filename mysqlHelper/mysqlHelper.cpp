#include "mysqlHelper.h"
mysqlHelper::mysqlHelper(std::string user,std::string password,std::string database,std::string ipaddress,int poolSize)
{
    for (int i=0;i<poolSize;i++)
        available.push_back(new mysqlHelperImp(user,password,database,ipaddress));
}
mysqlHelper::~mysqlHelper()
{
    for  (auto *connection:available)
        delete connection;
}