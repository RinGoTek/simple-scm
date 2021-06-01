//
// Created by a2447742618 on 2021/6/1.
//

#include "module_add.h"
#include"Database/file_system.h"
#include<sys/stat.h>
#include<sqlite3.h>
#include<vector>
#include<stack>
using namespace std;

static int callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    for (int i = 0; i < cnt; i++) cout << pName[i] << "=" << (pValue[i] ? pValue[i] : "NULL") << endl;
    return 0;
}

static vector<string>ignore;
static stack<string> walk_list;
static vector<string>file;

extern struct walk_return;

static int select_ignore_callback(void *NotUsed, int cnt, char **pValue, char **pName)//获取ignore信息的函数
{
    //把信息添加到ignore数组或者等待遍历的目录stack
    if(is_file(pValue[0])) ignore.emplace_back(pValue[0]);
    else if(is_dir(pValue[0])) walk_list.push(pValue[0]);
    return 0;
}

void module_add::add(char *path)
{
    if(is_file(path)) file.push_back(path);
    else if(is_dir(path)) file=walk_folder(path);
    else return;

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[500];

    //打开数据库
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据库打开失败：" << zErrMsg << endl;
        exit(1);
    } else {
        clog << "[INFO]数据库打开成功！" << endl;
    }

    //找到所有ignore的文件或文件夹
    sprintf(sql ,"SELECT Path From IgnoreList");
    rc = sqlite3_exec(db, sql, select_ignore_callback, nullptr, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    while(!walk_list.empty())
    {
        string current_dir = walk_list.top();
        walk_list.pop();

        extern struct walk_return wk;//do_walk_folder(current_dir);

        for (const auto &x:wk.dirs) walk_list.push(x);
        for (const auto &x:wk.files) ignore.emplace_back(x);
    }

    for(auto p:file)
    {
        struct stat buf;
        stat(p.c_str(), &buf);

        sprintf(sql, "INSERT INTO AddList (OriginPath,CreatedDateTime,UpdatedDateTime) VALUES ('%s','%s','%s')", path,buf.st_mtim, buf.st_mtim);
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "发生错误: " << zErrMsg << endl;
        }
    }

    sqlite3_close(db);

}