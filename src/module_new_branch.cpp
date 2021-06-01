//
// Created by a2447742618 on 2021/5/29.
//

#include "module_new_branch.h"
#include"Database/database.h"
#include<fstream>
#include<sqlite3.h>
#include<iostream>
#include<cstring>

using namespace std;


static char head_node[100];
int id;

static int callback(void *NotUsed, int cnt, char **pValue, char **pName)//用于创建新分支的回调函数，没有实际作用
{
    for (int i = 0; i < cnt; i++) cout << pName[i] << "=" << (pValue[i] ? pValue[i] : "NULL") << endl;
    return 0;
}

static int get_id(void *NotUsed, int cnt, char **pValue, char **pName)//用于获得新分支id的回调函数
{
    id = atoi(pValue[0]);
    return 0;
}

static int get_node(void *NotUsed, int cnt, char **pValue, char **pName)//用于获得当前节点的回调函数
{
    strcpy(head_node, pValue[0]);
    return 0;
}

void module_new_branch::create_branch(char *branch_name) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[500];

    //从文件中读取当前分支
    ifstream file(".simple-scm/current_branch.txt");

    int current_branch;
    file >> current_branch;
    file.close();
    //cout<<"current_branch="<<current_branch<<endl;

    //打开数据库
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);

    if (rc) {
        clog << "[ERROR]数据库加载失败：" << endl;
        exit(1);
    } else {
        clog << "[INFO]数据库加载成功！" << endl;
    }

    //获取当前节点
    sprintf(sql, "SELECT BranchHead FROM Branch WHERE ID='%d'", current_branch);

    rc = sqlite3_exec(db, sql, get_node, NULL, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]节点信息获取失败：" << zErrMsg << endl;
        exit(1);
    } else {
        clog << "[INFO]节点信息获取成功！" << endl;
    }

    //cout<<"head_node="<<head_node<<endl;

    //获取本地时间
    char tmp_time[100];
    auto tmpp = database::getCurrentTimeChar();
    strcpy(tmp_time, tmpp);
    free(tmpp);

    sprintf(sql,
            "INSERT INTO Branch (ID,Name,BranchRoot,BranchHead,CreatedDateTime,UpdatedDateTime) VALUES (NULL,'%s', (SELECT SHA FROM Node WHERE SHA='%s'), (SELECT SHA FROM Node WHERE SHA='%s'),'%s','%s')",
            branch_name, head_node, head_node, tmp_time, tmp_time);

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]新分支创建失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]新分支创建成功！" << endl;
    }

    //获取最后插入branch表的数据行的id
    sprintf(sql, "SELECT last_insert_rowid() from Branch");
    rc = sqlite3_exec(db, sql, get_id, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]新分支id获取失败 " << zErrMsg << endl;
    } else {
        clog << "[INFO]新分支id获取成功！" << endl;
    }
    //cout<<"id="<<id<<endl;
    tmpp = database::getCurrentTimeChar();
    strcpy(tmp_time, tmpp);
    free(tmpp);
    //cout<<"time="<<tmp_time<<endl;

    //连接新分支和根节点（根节点即当前节点）
    sprintf(sql,
            "INSERT INTO Node2Branch (ID,Node,Branch,CreatedDateTime) VALUES (NULL, (SELECT SHA FROM Node WHERE SHA='%s'), (SELECT ID FROM Branch WHERE ID='%d'), '%s')",
            head_node, id, tmp_time);

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]新分支与根节点连接失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]新分支与根节点连接成功！" << endl;
    }


    sqlite3_close(db);
}
