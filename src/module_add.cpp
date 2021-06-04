//
// Created by a2447742618 on 2021/6/1.
//

#include "module_add.h"
#include"Database/file_system.h"
#include"Database/database.h"
#include<sys/stat.h>
#include<sqlite3.h>
#include<vector>
#include<stack>
#include<algorithm>

using namespace std;

static int callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    for (int i = 0; i < cnt; i++) cout << pName[i] << "=" << (pValue[i] ? pValue[i] : "NULL") << endl;
    return 0;
}

static vector<string> ignore;
static stack<string> walk_list;
static vector<string> file;

static int select_ignore_callback(void *NotUsed, int cnt, char **pValue, char **pName)//获取ignore信息的函数
{
    //把信息添加到ignore数组或者等待遍历的目录stack
    if (is_file(pValue[0])) ignore.emplace_back(pValue[0]);
    else if (is_dir(pValue[0])) walk_list.push(pValue[0]);
    return 0;
}

void module_add::add(char *path) {
    if (is_file(path)) file.emplace_back(path);
    else if (is_dir(path)) file = walk_folder(path);
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
    sprintf(sql, "SELECT Path From IgnoreList");
    rc = sqlite3_exec(db, sql, select_ignore_callback, nullptr, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    //找到ignore文件夹下所有文件
    while (!walk_list.empty()) {
        string current_dir = walk_list.top();
        walk_list.pop();

        vector<string> tmp = walk_folder(current_dir);
        ignore.insert(ignore.end(), tmp.begin(), tmp.end());
    }

    //把不在ignore中的所有文件信息加入AddList中
    for (auto p:file) {
        auto it = find(ignore.begin(), ignore.end(), p);

        //在忽略列表中
        if (it != ignore.end()) continue;

        struct stat buf;
        stat(p.c_str(), &buf);

        sprintf(sql,
                "INSERT INTO AddList (OriginSHA,OriginPath,CreatedDateTime,UpdatedDateTime) VALUES ('%s','%s','%s','%s')",
                calculate_sha1(p), p.c_str(), database::getTimeChar(buf.st_mtime), database::getTimeChar(buf.st_mtime));
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "发生错误: " << zErrMsg << endl;
            exit(1);
        }
    }

    sqlite3_close(db);

}