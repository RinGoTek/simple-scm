//
// Created by a2447742618 on 2021/5/29.
//

#include "model_new_branch.h"
#include"database.h"
#include<fstream>
#include<sqlite3.h>
#include<iostream>
#include<cstring>
using namespace std;

static char head_node[100];
int id;

static int callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    for (int i = 1; i <= cnt; i++) cout << pName << "=" << (pValue[i] ? pValue[i] : "NULL") << endl;
    return 0;
}

static int get_id(void *NotUsed, int cnt, char **pValue, char **pName) {
    id= atoi(pValue[0]);
    return 0;
}

static int get_node(void *NotUsed, int cnt, char **pValue, char **pName)
{
    strcpy(head_node,pValue[0]);
    return 0;
}

void model_new_branch::creat_branch(char branch_name[])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    ifstream file("current_branch.txt");
    int current_branch;
    file>>current_branch;
    file.close();
    //clog<<"current_branch="<<current_branch<<endl;

    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if(rc)
    {
        clog<<"[ERROR]数据库打开失败："<<endl;
        exit(1);
    }
    else
    {
        clog<<"[INFO]数据库打开成功！"<<endl;
    }

    sprintf(sql,  "SELECT BranchHead FROM Branch WHERE ID='%d'",current_branch);
    rc = sqlite3_exec(db, sql, get_node, NULL, &zErrMsg);
    if(rc != SQLITE_OK)
    {
        clog<<"[ERROR]节点信息获取失败："<<zErrMsg<<endl;
        exit(1);
    }
    else
    {
        clog<<"[INFO]节点信息获取成功！"<<endl;
    }
    clog<<"head_node="<<head_node<<endl;
    char* tmp_time = database::getCurrentTimeChar();

    sprintf(sql,  "INSERT INTO Branch (ID,Name,BranchRoot,BranchHead,CreatedDateTime,UpdatedDateTime) VALUES (NULL,'%s', (SELECT SHA FROM Node WHERE SHA='%s'), (SELECT SHA FROM Node WHERE SHA='%s'),'%s','%s')",branch_name,head_node,head_node,tmp_time,tmp_time);

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]新分支创建失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]新分支创建成功！" << endl;
    }

    sprintf(sql, "SELECT last_insert_rowid() from Branch");//获取最后插入数据的id
    rc = sqlite3_exec(db, sql, get_id, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]id获取失败 " << zErrMsg << endl;
    } else {
        clog << "[INFO]id获取成功！" << endl;
    }
    //clog<<"id="<<id<<endl;
    tmp_time = database::getCurrentTimeChar();

    sprintf(sql,  "INSERT INTO Node2Branch (ID,Node,Branch,CreatedDateTime) VALUES (NULL, (SELECT SHA FROM Node WHERE SHA='%s'), (SELECT ID FROM Branch WHERE ID='%d'), '%s')",head_node,id,tmp_time);

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]新分支与根节点连接失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]新分支与根节点连接成功！" << endl;
    }

    sqlite3_close(db);
}
