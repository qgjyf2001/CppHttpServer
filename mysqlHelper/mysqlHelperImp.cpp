#include "mysqlHelper.h"
mysqlHelperImp::mysqlHelperImp(std::string user,std::string password,std::string database,std::string ipaddress)
{
    this->user=user;
    this->password=password;
    this->database=database;
    this->ipaddress=ipaddress;
    connection=mysql_init(NULL);
    if (!mysql_real_connect(connection,ipaddress.c_str(),user.c_str(),password.c_str(),database.c_str(),0,NULL,0))
        throw std::runtime_error("sql connect error");

} 
std::vector<sqlResult> mysqlHelperImp::_query(std::string sql)
{
    std::vector<sqlResult> result;
    auto res=mysql_query(connection,sql.c_str());
    if (res)
    {
        throw std::runtime_error("sql query failed");
    }
    auto *resPtr=mysql_store_result(connection);
    auto column=mysql_num_fields(resPtr);
    auto row=mysql_num_rows(resPtr);
    MYSQL_FIELD* field;
    std::vector<std::string> columnName;
    std::vector<enum_field_types> ftypes;
    while (field = mysql_fetch_field(resPtr))
    {
        columnName.emplace_back(std::string(field->name));
        ftypes.emplace_back(field->type);
    }
    for (int i=0;i<row;i++)
    {
        sqlResult _result;
        auto *res=mysql_fetch_row(resPtr);
        for (int j=0;j<column;j++)
        {
            _result[columnName[j]]=sqlValue(std::string(res[j]),ftypes[j]);
        }
        result.push_back(_result);
    }
    return result;
}
mysqlHelperImp::~mysqlHelperImp()
{
    mysql_close(connection);
}