//
// Created by login on 2021/5/16.
//

#include<memory>
#include"headers/cache.h"
#include "headers/init-db.h"
#include<string>
#include<stdlib.h>
#include<cstring>
#include<cstdio>
#include<sqlite3.h>

using namespace std;

char cc[16] = {'0', '1', '2', '3', '4', '5','6','7','8','9','a','b','c','d','e','f'};

init_db::init_db()
{
};

//初始化数据仓库的函数
void init_db::do_init() {

    clog << "正在初始化存储库" << endl;

    if (mkdir(".simple-scm", 0700) < 0) {
        if(errno == EEXIST)
        {
            clog<<"存储库已经存在"<<endl;
            exit(1);
        }
        else
        {
            cerr << "创建存储库失败" << endl;
            exit(1);
        }

    }

    //创建根目录
    clog<<"正在创建数据根目录..."<<endl;


    string sha1_dir = DEFAULT_DB_ENVIRONMENT;
    clog<<"正在创建数据目录..."<<endl;

    if(mkdir(sha1_dir.c_str(), 0700)<0)
    {
        if(errno != EEXIST)
        {
            cout<<"发生错误"<<endl;
            exit(1);
        }
    }

    for(char i : cc)
    {
        for(char j : cc)
        {
            if(mkdir((sha1_dir + "/" + i + j).c_str(), 0700) < 0)
            {
                if(errno != EEXIST)
                {
                    cerr<<"发生错误： "<<sha1_dir + "/" + i + j<<endl;
                    exit(1);
                }
            }
        }
    }

    clog<<"创建数据目录成功！"<<endl;

}

void database_init()
{
    sqlite3 *db;
    char *zErrMsg=0;
    int rc;
    char *sql;

    rc=sqlite3_open(".simple-scm/simple-scm.db",&db);

    if(rc)
    {
        clog<<"数据库创建失败！"<<endl;
        exit(1);
    }
    else
    {
        clog<<"数据库创建成功！"<<endl;
    }

    sql="CREATE TABLE Objects("\
        "CompressedSHA CHAR(40) PRIMARY KEY,"\
        "OriginSHA CHAR(40),"\
        "OriginPath TEXT(500),"\
        "CompressedPath TEXT(500),"\
        "CreatedDateTime DATETIME,"\
        "UpdatedDateTime DATETIME);";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
        clog<<"数据表Object创建失败，原因是"<<zErrMsg<<endl;
    }
    else
    {
        clog<<"数据表Object创建成功！"<<endl;
    }

    sql="CREATE TABLE Node("\
        "SHA CHAR(40) PRIMARY KEY,"\
        "CreatedDateTime DATETIME,"\
        "Parent ForeignKey,"\
        "Message TEXT(1000);";

    rc =  sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
        clog<<"数据表Node创建失败，原因是"<<zErrMsg<<endl;
    }
    else
    {
        clog<<"数据表Node创建成功！"<<endl;
    }



}

