#ifndef MYSQLHELPER_H
#define MYSQLHELPER_H
#include <iostream>
#include <type_traits>
#include <vector>
#include <map>
#include <tuple>

#include <mysql/mysql.h>

#include "jsonParser.h"
#include "threadPool.h"
#include "safeVector.h"
struct sqlDate
{
    sqlDate(int year,int month,int day,int hour=0,int minute=0,int second=0):year(year),month(month),day(day),hour(hour),minute(minute),second(second)
    {

    }
    std::string toString()
    {
        char result[30];
        sprintf(result,"%04d-%02d-%02d %02d:%02d:%02d",year,month,day,hour,minute,second);
        return std::string(result);
    }
    int year,month,day,hour,minute,second;
};
struct  sqlValue
{
    using sqlType=enum_field_types;
    sqlValue(std::string data,sqlType type)
    {
        this->type=type;
        this->data=data;
    }
    sqlValue():sqlValue("",MYSQL_TYPE_NULL)
    {
    }
    bool isString()
    {
        return type==MYSQL_TYPE_STRING||type==MYSQL_TYPE_VARCHAR||type==MYSQL_TYPE_DATE||type==MYSQL_TYPE_DATETIME||type==MYSQL_TYPE_VAR_STRING;
    }        
    std::string data;
    private:
        sqlType type;
};
using sqlResult=std::map<std::string,sqlValue>;
class mysqlHelperImp
{
public:
    mysqlHelperImp(std::string user,std::string password,std::string database,std::string ipaddress="localhost");
    ~mysqlHelperImp();
    template<typename... types>
    std::vector<sqlResult> query(std::string sql,types... args)
    {
       modify<1>(sql,args...);
       return _query(sql);
    }
    template<typename... types>
    JsonParser queryJSON(std::string sql,types... args)
    {
        auto result=query(sql,args...);
        std::string arr="[]";
        JsonParser jsonArray(&arr,JsonParser::ARRAY);
        for (auto &&each:result)
        {
            JsonParser t;
            for (auto &&[name,value]:each)
            {
                if (value.isString())
                    t[name]=value.data;
                else
                    t[name]=std::atoi(value.data.c_str());
            }
            jsonArray.add(t);
        }
        return jsonArray;
    }
    template<typename... types>
    void perform(std::string sql,types... args)
    {
       modify<1>(sql,args...);
       auto res=mysql_query(connection,sql.c_str());
        if (res)
        {
            throw std::runtime_error("sql query failed");
        }
    }
protected:
    std::vector<sqlResult> _query(std::string sql);
private:
    enum  queryType
    {
        INT,STRING,DATE
    };
    template<typename T>
    struct  getQueryType
    {
        using rT=typename std::remove_const<T>::type;
        static constexpr auto value=std::is_same<rT,int>::value?INT:std::is_same<rT,sqlDate>::value?DATE:STRING;
    };
    std::string toString(std::string str)
    {
        return str;
    }
    std::string toString(int num)
    {
        return std::to_string(num);
    }
    template<int n>
    void modify(std::string& sql)
    {

    }
    template<int n,typename T,typename... others>
    void modify(std::string& sql,T param,others... args)
    {
        constexpr auto type=getQueryType<T>::value;
        std::string str="%"+std::to_string(n);
        int len=str.length();
        if constexpr(type==INT)
        {
            std::string::size_type pos;
            while ((pos=sql.find(str))!=sql.npos)
            {
                sql.replace(pos,len,std::to_string(param));
            }
        }
        else if constexpr(type==DATE)
        {
            std::string::size_type pos;
            while ((pos=sql.find(str))!=sql.npos)
            {
                sql.replace(pos,len,std::string("'")+param.toString()+std::string("'"));
            }
        }
        else
        {
            std::string::size_type pos;
            while ((pos=sql.find(str))!=sql.npos)
            {
                sql.replace(pos,len,std::string("'")+param+std::string("'"));
            }
        }
        modify<n+1>(sql,args...);
    }
    MYSQL *connection;
    std::string user,password,database,ipaddress;
};
class mysqlHelper
{
public:
    mysqlHelper(std::string user,std::string password,std::string database,std::string ipaddress="localhost",int poolSize=4);
    template<typename... types>
    JsonParser queryJSON(std::string sql,types... args)
    {
        auto *connection=getConnection();
        auto result=connection->queryJSON(sql,args...);
        available.push_back(connection);
        return result;
    }
    template<typename... types>
    std::vector<sqlResult> query(std::string sql,types... args)
    {
        auto *connection=getConnection();
        auto result=connection->query(sql,args...);
        available.push_back(connection);
        return result;
    }
    template<typename... types>
    void perform(std::string sql,types... args)
    {
        auto *connection=getConnection();
        connection->perform(sql,args...);
        available.push_back(connection);
    }
    ~mysqlHelper();
private:
    mysqlHelperImp* getConnection()
    {
        std::lock_guard<std::mutex> lck(lock);
        while (!available.size());
        auto *connection=available.pop();
        return connection;
    }
    std::mutex lock;
    safeVector<mysqlHelperImp*> available;
};
#endif