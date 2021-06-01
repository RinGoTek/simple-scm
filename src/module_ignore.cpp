//
// Created by login on 2021/6/1.
//

#include "module_ignore.h"
#include<string>
#include<iostream>
#include<sqlite3.h>
#include"Database/file_system.h"
#include"Database/database.h"
using namespace std;

static string error_info_unique_constraint_failed = "UNIQUE constraint failed: IgnoreList.Path";

static int check_in_repository_callback(void *NotUsed, int cnt, char **pValue, char **pName)//获取ignore信息的函数
{
    if(int(*pValue[0]) - '0'>0)
    {

        return 1;
    }
    return 0;
}

bool is_in_repository(sqlite3* &db, const string& path)
{
    char *zErrMsg = 0;
    int rc;
    char sql[500];
    //已经被加到存储库中的文件不能被ignore
    sprintf(sql, "SELECT COUNT(*) FROM Objects WHERE OriginPath='%s'", path.c_str());
    rc = sqlite3_exec(db, sql, check_in_repository_callback, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout<<zErrMsg<<endl;
        return true;
    }
    return false;
}


//把路径加入到忽略列表
void module_ignore::do_ignore(string path) {

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);

    if (rc) {
        cerr << "[ERROR]数据库加载失败！" << endl;
        exit(1);
    } else {
        clog << "[INFO]数据库加载成功！" << endl;
    }

    char sql[500];


    if(is_file(path))
    {
        //不能把.simple-scm添加到ignore，这是保留路径
        string tmpp = ".simple-scm";
        bool flag = false;
        for(int i=0;i<tmpp.length();++i)
        {
            if(tmpp[i] == path[i])
                flag = true;
            else
            {
                flag = false;
                break;
            }
        }

        if(flag)
        {
            cerr<<"[ERROR]系统保留的路径"<<endl;
            exit(0);
        }
        if(is_in_repository(db, path))
        {
            clog<<"[INFO] "<<path<<" 已经存在于存储库中，不可被忽略！"<<endl;
            sqlite3_close(db);
            exit(0);
        }


        sprintf(sql, "INSERT INTO IGNORELIST (Path,CreatedDateTime) VALUES ('%s', '%s')", path.c_str(), database::getCurrentTimeChar());
        rc = sqlite3_exec(db, sql, 0, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            if(zErrMsg == error_info_unique_constraint_failed)
                clog<<"[INFO] "<<path<<" 已经存在Ignore中"<<endl;
            else
                cerr<<"[ERROR]处理出错： "<<zErrMsg<<endl;
        }
        else
            clog<<"[INFO]成功将 "<<path<<" 添加到ignore列表！"<<endl;

    }

    else if(is_dir(path))
    {
        //不能把.simple-scm添加到ignore，这是保留路径
        if(path==".simple-scm")
        {
            cerr<<"[ERROR]系统保留的路径"<<endl;
            exit(0);
        }

        if(is_in_repository(db, path))
        {
            clog<<"[INFO] "<<path<<" 已经存在于存储库中，不可被忽略！"<<endl;
            sqlite3_close(db);
            exit(0);
        }


        sprintf(sql, "INSERT INTO IGNORELIST (Path,CreatedDateTime) VALUES ('%s', '%s')", path.c_str(), database::getCurrentTimeChar());
        rc = sqlite3_exec(db, sql, 0, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            if(zErrMsg == error_info_unique_constraint_failed)
                clog<<"[INFO] "<<path<<" 已经存在Ignore中"<<endl;
            else
                cerr<<"[ERROR]处理出错： "<<zErrMsg<<endl;
        }
        else
            clog<<"[INFO]成功将 "<<path<<" 添加到ignore列表！"<<endl;
    }
    else
    {
        cerr<<"[ERROR]请输入正确的路径！"<<endl;
    }


    sqlite3_close(db);
}
