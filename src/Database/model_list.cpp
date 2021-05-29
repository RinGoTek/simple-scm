//
// Created by a2447742618 on 2021/5/29.
//

#include "model_list.h"
#include<sqlite3.h>
#include<iostream>
using namespace std;

static int callback(void *NotUsed, int cnt, char **pValue, char **pName)
{
    for(int i=0;i<cnt;i++)
    {
        cout<<pName[i]<<"="<<(pValue[i] ? pValue[i] : "NULL") <<endl;
    }
    cout<<endl;
    return 0;
}

void model_list::list()
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if(rc != SQLITE_OK)
    {
        clog<<"[ERROR]数据库打开失败："<<zErrMsg<<endl;
        exit(1);
    }
    else
    {
        clog<<"[INFO]数据库打开成功！"<<endl;
    }

    //sprintf(sql,"SELECT * FROM Branch"); 错误，原因未知
    sql="SELECT * FROM Branch";
    rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);

    if(rc != SQLITE_OK)
    {
        clog<<"[ERROR]发生错误："<<zErrMsg<<endl;
    }
    sqlite3_close(db);
}

