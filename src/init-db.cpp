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

    clog << "[INFO]正在初始化存储库" << endl;

    if (mkdir(".simple-scm", 0700) < 0) {
        if(errno == EEXIST)
        {
            clog<<"[INFO]存储库已经存在"<<endl;
            exit(1);
        }
        else
        {
            cerr << "[ERROR]创建存储库失败" << endl;
            exit(1);
        }

    }

    //创建根目录
    clog<<"[INFO]正在创建数据根目录..."<<endl;


    string sha1_dir = DEFAULT_DB_ENVIRONMENT;
    clog<<"[INFO]正在创建数据目录..."<<endl;

    if(mkdir(sha1_dir.c_str(), 0700)<0)
    {
        if(errno != EEXIST)
        {
            cout<<"[ERROR]发生错误"<<endl;
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
                    cerr<<"[ERROR]发生错误： "<<sha1_dir + "/" + i + j<<endl;
                    exit(1);
                }
            }
        }
    }

    clog<<"[INFO]创建数据目录成功！"<<endl;

}

static int callback(void *NotUsed,int argc, char **argv,char **azColName)
{
    for(int i=1;i<=argc;i++) cout<<azColName<<"="<<(argv[i] ? argv[i] : "NULL" )<< endl;
    return 0;
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
        clog<<"[ERROR]数据库创建失败！"<<endl;
        exit(1);
    }
    else
    {
        clog<<"[INFO]数据库创建成功！"<<endl;
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
        clog<<"[ERROR]数据表Object创建失败，原因是"<<zErrMsg<<endl;
    }
    else
    {
        clog<<"[INFO]数据表Object创建成功！"<<endl;
    }

    sql="CREATE TABLE Node("\
        "SHA CHAR(40) PRIMARY KEY,"\
        "CreatedDateTime DATETIME,"\
        "Parent CHAR(40),"\
        "Message TEXT(1000)"\
        "FOREIGN KEY (Parent) REFERENCE Node(SHA);";

    rc =  sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
        clog<<"[ERROR]数据表Node创建失败，原因是"<<zErrMsg<<endl;
    }
    else
    {
        clog<<"[INFO]数据表Node创建成功！"<<endl;
    }

    sql="CREATE TABLE Obj2Node("\
        "ID INTEGER PRIMARY KEY,"\
        "File CHAR(40),"\
        "NODE CHAR(40),"\
        "Mode INTEGER,"\
        "CreatedDateTime DATETIME,"\
        "FOREIGN KEY (File) REFERENCE Objects(CompressedSHA),"\
        "FOREIGN KEY (NODE) REFERENCE Node(SHA);";

    rc =  sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
        clog<<"[ERROR]数据表Obj2Node创建失败，原因是"<<zErrMsg<<endl;
    }
    else
    {
        clog<<"[INFO]数据表Obj2Node创建成功！"<<endl;
    }

    sql="CREATE TABLE Branch("\
        "ID INTEGER PRIMARY KEY,"\
        "Name TEXT(40),"\
        "BranchRoot CHAR(40),"\
        "CreatedDateRTime DATETIME,"\
        "UpdatedDateTime DATETIME,"\
        "FOREIGN KEY BranchRoot REFERENCE Node(SHA);";

    rc =  sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
        clog<<"[ERROR]数据表Branch创建失败，原因是"<<zErrMsg<<endl;
    }
    else
    {
        clog<<"[INFO]数据表Branch创建成功！"<<endl;
    }

    sql="CREATE TABLE Node2Branch("\
        "ID INTEGER PRIMARY KEY("\
        "Node CHAR(40),"\
        "Branch INTEGER,"\
        "CreatedDateTime DATETIME,"\
        "FOREIGN KEY Node REFERENCE Node(SHA),"\
        "FOREIGN KEY Branch REFERENCE Branch(ID);";

    rc =  sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
        clog<<"[ERROR]数据表Node2Branch创建失败，原因是"<<zErrMsg<<endl;
    }
    else
    {
        clog<<"[INFO]数据表Node2Branch创建成功！"<<endl;
    }

    sql="CREATE TABLE AddList("\
        "ID INTEGER PRIMARY KEY,"\
        "OriginSHA CHAR(40),"\
        "OriginPath TEXT(500),"\
        "CreatedDateTime DATETIME,"\
        "UpdatedDateTime DATETIME;";

    rc =  sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
        clog<<"[ERROR]数据表AddList创建失败，原因是"<<zErrMsg<<endl;
    }
    else
    {
        clog<<"[INFO]数据表AddList创建成功！"<<endl;
    }
}

