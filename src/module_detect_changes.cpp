//
// Created by a2447742618 on 2021/5/28.
//

#include "module_detect_changes.h"
#include"Database/file_system.h"
#include"Database/database.h"
#include<sqlite3.h>
#include<iostream>
#include<cstdlib>
#include<fstream>
#include<cstring>
#include<stack>
#include<vector>
#include<algorithm>

using namespace std;

static char head_node[100], root_node[100], node[100];
static map<string, char *> object_updated_time; //储存从文件哈希值到文件更改时间的映射
static map<string, bool> vis;
static vector<string> object;
static stack<string> walk_list;
static vector<string> ignore_object;
static vector<string> local_object;

void init() {
    object.clear();
    object_updated_time.clear();
    vis.clear();
    ignore_object.clear();
    local_object.clear();
}

static int get_node(void *NotUsed, int cnt, char **pValue, char **pName)//用来获得当前分支头节点和根节点的回调函数
{
    strcpy(root_node, pValue[0]);
    strcpy(head_node, pValue[1]);
    return 0;
}

static int update_node(void *NotUsed, int cnt, char **pValue, char **pName)//将节点更新为父亲节点的回调函数
{
    strcpy(node, pValue[0]);
    return 0;
}

static int get_object(void *NotUsed, int cnt, char **pValue, char **pName)//获得当前节点包含的object的回调函数
{
    if (vis[pValue[0]]) return 0;
    vis[pValue[0]] = 1;
    if (atoi(pValue[1]) != -1) object.push_back(pValue[1]);
    return 0;
}

static int select_ignore_callback(void *NotUsed, int cnt, char **pValue, char **pName)//获取ignore信息的函数
{
    //把信息添加到ignore数组或者等待遍历的目录stack
    if (is_file(pValue[0])) ignore_object.emplace_back(pValue[0]);
    else if (is_dir(pValue[0])) walk_list.push(pValue[0]);
    return 0;
}

static int get_updated_time(void *NotUsed, int cnt, char **pValue, char **pName)//获取文件对应的更新时间
{
    object_updated_time[pValue[0]] = pValue[1];
    return 0;
}

detect_info module_detect_changes::detect_changes() {

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    init();
    //从文件中读取当前分支
    ifstream file(".simple-scm/current_branch.txt");
    int current_branch;
    file >> current_branch;
    file.close();

    //打开数据库
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据库打开失败：" << endl;
        exit(1);
    } else {
        clog << "[INFO]数据库打开成功！" << endl;
    }

    //获得当前分支头节点，即当前节点
    sprintf(sql, "SELECT BranchRoot,BranchHead FROM Branch WHERE ID='%d'", current_branch);
    rc = sqlite3_exec(db, sql, get_node, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]节点信息获取失败：" << zErrMsg << endl;
        exit(1);
    } else {
        clog << "[INFO]节点信息获取成功！" << endl;
    }


    strcpy(node, head_node);
    //节点不断向根节点更新从而获得所有包含的object
    while (!strcmp(node, root_node)) {
        sprintf(sql, "SELECT File,Mode FROM Obj2Node WHERE Node='%s'", node);
        rc = sqlite3_exec(db, sql, get_object, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }


        sprintf(sql, "SELECT Parent FROM Node WHERE SHA='%s'", node);
        rc = sqlite3_exec(db, sql, update_node, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }

    //找到所有ignore的文件或文件夹
    sprintf(sql, "SELECT Path From IgnoreList");
    rc = sqlite3_exec(db, sql, select_ignore_callback, nullptr, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    //获得所有ignore文件夹下的文件
    while (!walk_list.empty()) {
        string current_dir = walk_list.top();
        walk_list.pop();

        vector<string> tmp = walk_folder(current_dir);
        ignore_object.insert(ignore_object.end(), tmp.begin(), tmp.end());
    }

    //获得本地所有文件路径
    local_object = walk_folder(cwd);

    for (auto &p:local_object) {
        auto it = find(ignore_object.begin(), ignore_object.end(), p);

        //在忽略列表中
        if (it != ignore_object.end()) continue;

        sprintf(sql, "SELECT SHA,UpdatedDateTime FROM Object WHERE SHA='%s'", p.c_str());

        rc = sqlite3_exec(db, sql, get_updated_time, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }

    }

    detect_info sav;

    for (auto &p:local_object) {
        auto it = find(ignore_object.begin(), ignore_object.end(), p);

        //在忽略列表中
        if (it != ignore_object.end()) continue;

        struct stat buf;
        if (stat(p.c_str(), &buf) == -1)//找不到该文件
        {
            sav.del.emplace_back(p);
            continue;
        }

        char *time1;
        strcpy(time1,object_updated_time[p]);
        if (strcmp(time1, database::getTimeChar(buf.st_mtime))) sav.change.emplace_back(p);//文件更新时间与表中不同
    }

    sqlite3_close(db);

    return sav;
}