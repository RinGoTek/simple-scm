//
// Created by login on 2021/6/1.
//

#include "module_ignore.h"
#include<string>
#include<iostream>
#include<sqlite3.h>
#include"Database/file_system.h"
#include"Database/database.h"
#include<regex>

using namespace std;

static string error_info_unique_constraint_failed = "UNIQUE constraint failed: IgnoreList.Path";

static int check_in_repository_callback(void *NotUsed, int cnt, char **pValue, char **pName)//获取ignore信息的函数
{
    if (int(*pValue[0]) - '0' > 0) {

        return 1;
    }
    return 0;
}


/**
 * 这是获取ignoreList的全部内容的回调函数
 * @param NotUsed
 * @param cnt
 * @param pValue
 * @param pName
 * @return
 */
vector<string> ignorePathList;

static int select_all_from_ignore_list_callback(void *NotUsed, int cnt, char **pValue, char **pName)//获取ignore信息的函数
{
    ignorePathList.emplace_back(*pValue);
    return 0;
    //ignorePathList.emplace_back((*pValue[0]));
}

/**
 * 这是检测文件是否存在于ignoreList的回调函数
 * @param NotUsed
 * @param cnt
 * @param pValue
 * @param pName
 * @return
 */
bool is_to_deignore_file_exists = false;

static int deignore_file_select_callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    is_to_deignore_file_exists = true;
    return 0;
}

bool is_in_repository(sqlite3 *&db, const string &path) {
    char *zErrMsg = 0;
    int rc;
    char sql[500];
    //已经被加到存储库中的文件不能被ignore
    sprintf(sql, "SELECT COUNT(*) FROM Objects WHERE OriginPath='%s'", path.c_str());
    rc = sqlite3_exec(db, sql, check_in_repository_callback, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << zErrMsg << endl;
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


    if (is_file(path)) {
        //不能把.simple-scm添加到ignore，这是保留路径
        string tmpp = ".simple-scm";
        bool flag = false;
        for (int i = 0; i < tmpp.length(); ++i) {
            if (tmpp[i] == path[i])
                flag = true;
            else {
                flag = false;
                break;
            }
        }

        if (flag) {
            cerr << "[ERROR]系统保留的路径" << endl;
            exit(0);
        }
        if (is_in_repository(db, path)) {
            clog << "[INFO] " << path << " 已经存在于存储库中，不可被忽略！" << endl;
            sqlite3_close(db);
            exit(0);
        }


        sprintf(sql, "INSERT INTO IGNORELIST (Path,CreatedDateTime) VALUES ('%s', '%s')", path.c_str(),
                database::getCurrentTimeChar());
        rc = sqlite3_exec(db, sql, 0, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            if (zErrMsg == error_info_unique_constraint_failed)
                clog << "[INFO] " << path << " 已经存在Ignore中" << endl;
            else
                cerr << "[ERROR]处理出错： " << zErrMsg << endl;
        } else
            clog << "[INFO]成功将 " << path << " 添加到ignore列表！" << endl;

    } else if (is_dir(path)) {
        //不能把.simple-scm添加到ignore，这是保留路径
        if (path == ".simple-scm") {
            cerr << "[ERROR]系统保留的路径" << endl;
            exit(0);
        }

        if (is_in_repository(db, path)) {
            clog << "[INFO] " << path << " 已经存在于存储库中，不可被忽略！" << endl;
            sqlite3_close(db);
            exit(0);
        }


        sprintf(sql, "INSERT INTO IGNORELIST (Path,CreatedDateTime) VALUES ('%s', '%s')", path.c_str(),
                database::getCurrentTimeChar());
        rc = sqlite3_exec(db, sql, 0, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            if (zErrMsg == error_info_unique_constraint_failed)
                clog << "[INFO] " << path << " 已经存在Ignore中" << endl;
            else
                cerr << "[ERROR]处理出错： " << zErrMsg << endl;
        } else
            clog << "[INFO]成功将 " << path << " 添加到ignore列表！" << endl;
    } else {
        cerr << "[ERROR]请输入正确的路径！" << endl;
    }


    sqlite3_close(db);
}

//取消ignore
//要传入相对路径
void module_ignore::deIgnore(std::string path) {
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

    if (is_file(path)) {
        sprintf(sql, "SELECT Path from IgnoreList WHERE Path = '%s'", path.c_str());

        rc = sqlite3_exec(db, sql, deignore_file_select_callback, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "[ERROR]处理出错： " << zErrMsg << endl;
        }
        //传入的文件不在忽略列表中
        if (!is_to_deignore_file_exists) {
            cerr << "[ERROR]传入的文件路径不在忽略列表中" << endl;
            sqlite3_close(db);
            exit(0);
        }

        sprintf(sql, "DELETE FROM IgnoreList WHERE Path = '%s'", path.c_str());
        rc = sqlite3_exec(db, sql, 0, nullptr, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]处理出错： " << zErrMsg << endl;
        } else {
            clog << "[INFO]将 " << path << "移出Ignore列表" << endl;
            exit(0);
        }

    } else if (is_dir(path)) {
        sprintf(sql, "SELECT Path FROM IgnoreList");
        rc = sqlite3_exec(db, sql, select_all_from_ignore_list_callback, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "[ERROR]处理出错： " << zErrMsg << endl;
        }

        if (path[path.length() - 1] == '/')
            path = path.substr(0, path.length() - 1);


        for (const string &x:ignorePathList) {

            bool found = false;
            if (x.length()<path.length())
                continue;

            if(path == x.substr(0, path.length()))
                found = true;

            if (found) {

                    sprintf(sql, "DELETE FROM IgnoreList WHERE Path = '%s'", x.c_str());
                    rc = sqlite3_exec(db, sql, 0, nullptr, &zErrMsg);
                    if (rc != SQLITE_OK) {
                        cerr << "[ERROR]处理出错： " << zErrMsg << endl;
                        cerr << "[ERROR]文件：" << x << endl;
                    } else {
                        clog << "[INFO]将 " << x << " 移出Ignore列表" << endl;

                    }


            }

        }
        cout<<"[INFO]处理完成"<<endl;


    } else {
        cerr << "[ERROR]请输入正确的路径！" << endl;
    }

    sqlite3_close(db);
}
