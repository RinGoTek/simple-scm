//
// Created by windsoul on 2021/6/2.
//

#include "module_checkout.h"
#include <iostream>
#include <sqlite3.h>
#include <cstring>

using namespace std;
struct branch_message
{
    int id;
    char name[100], root[100], head[100], created_time[100], updated_time[100];
};

branch_message tmp;//存储所要切换的分支的信息

static char head_node[100], root_node[100];//分支的目前结点和根结点

static int callback(void *NotUsed,int cnt,char **pValue,char **pName){
    tmp.id = atoi(pValue[0]);
    strcpy(tmp.name, pValue[1]);
    strcpy(tmp.root, pValue[2]);
    strcpy(tmp.head, pValue[3]);
    strcpy(tmp.created_time, pValue[4]);
    strcpy(tmp.updated_time, pValue[5]);
    return 0;
}

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
void module_checkout::checkout_switch_branch(char *switch_branch_name) {
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

    clog << "[INFO]正在切换分支..." << endl;

    //获得branch表中所要切换的表的信息
    sprintf(sql, "SELECT * FROM Branch WHERE NAME ='%s';",switch_branch_name);
    rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    //输出切换的表的信息
    cout <<switch_branch_name<<":\n"<< "ID=" << tmp.id << endl << "Name=" << tmp.name << endl << "BranchRoot=" << tmp.root << endl
         << "BranchHead=" << tmp.head << endl << "CreatedDateTime=" << tmp.created_time << endl
         << "UpdatedDateTime=" << tmp.updated_time << endl << endl << tmp.name << "分支提交记录为：" << endl;

    strcpy(head_node, tmp.head);
    strcpy(root_node, tmp.root);

    //边输出节点信息边向上更新，直到根节点为止
    while (strcmp(head_node, root_node) != 0) {
        sprintf(sql, "SELECT * FROM Node WHERE SHA='%s';", head_node);
        rc = sqlite3_exec(db, sql, get_node, NULL, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }
    //输出根节点信息
    sprintf(sql, "SELECT * FROM Node WHERE SHA='%s';", root_node);
    rc = sqlite3_exec(db, sql, out_root, NULL, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    //checkout切换分支成功
    clog<<"[INFO]切换到分支"<<switch_branch_name<<"成功！"<<endl;

    sqlite3_close(db);
}