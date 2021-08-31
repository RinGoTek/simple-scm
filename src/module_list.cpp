//
// Created by a2447742618 on 2021/5/29.
//

#include "module_list.h"
#include<sqlite3.h>
#include<iostream>
#include<vector>
#include<cstring>
#include "Database/database.h"
#include"headers/global.h"

using namespace std;

struct branch1 {
    int id;
    char name[100], root[100], head[100], created[100], updated[100];
};
vector<branch1> sav;//保存分支信息

static int callback(void *NotUsed, int cnt, char **pValue, char **pName)//用于获得branch表所有信息的回调函数
{
    branch1 tmp;
    tmp.id = atoi(pValue[0]);
    strcpy(tmp.name, pValue[1]);
    strcpy(tmp.root, pValue[2]);
    strcpy(tmp.head, pValue[3]);
    strcpy(tmp.created, pValue[4]);
    strcpy(tmp.updated, pValue[5]);
    sav.push_back(tmp);
    return 0;
}

static char head_node[100], root_node[100];

static int get_node(void *NotUsed, int cnt, char **pValue, char **pName)//输出节点信息并向上更新节点的回调函数
{
    cout << pValue[0] << ": " << pValue[3] << " " << pValue[1] << endl;
    strcpy(head_node, pValue[2]);
    return 0;
}

static int out_root(void *NotUsed, int cnt, char **pValue, char **pName)//输出根节点信息的回调函数
{
    cout << pValue[0] << ": " << pValue[3] << " " << pValue[1] << endl << endl;
    return 0;
}

void module_list::list() {
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
        if(DEV_MODE)
            clog << "[INFO]数据库打开成功！" << endl;
    }

    //获得branch表中所有信息
    sprintf(sql, "SELECT * FROM Branch");
    rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    //遍历分支，输出分支信息后输出节点记录
    for (auto p:sav) {
        cout << "ID=" << p.id << endl << "Name=" << p.name << endl << "BranchRoot=" << p.root << endl
             << "BranchHead=" << p.head << endl << "CreatedDateTime=" << p.created << endl
             << "UpdatedDateTime=" << p.updated << endl << endl << p.name << "分支提交记录为：" << endl;

        strcpy(head_node, p.head);
        strcpy(root_node, p.root);
        //边输出节点信息边向上更新，直到根节点为止
        while (strcmp(head_node, root_node) != 0) {
            sprintf(sql, "SELECT * FROM Node WHERE SHA='%s'", head_node);
            rc = sqlite3_exec(db, sql, get_node, NULL, &zErrMsg);

            if (rc != SQLITE_OK) {
                cerr << "[ERROR]发生错误：" << zErrMsg << endl;
                exit(1);
            }
        }

        //输出根节点信息
        sprintf(sql, "SELECT * FROM Node WHERE SHA='%s'", root_node);
        rc = sqlite3_exec(db, sql, out_root, NULL, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }

    sqlite3_close(db);
}

void module_list::help() {

}

