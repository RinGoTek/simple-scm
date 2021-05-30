//
// Created by a2447742618 on 2021/5/28.
//

#include "model_detect_changes.h"
#include<sqlite3.h>
#include<iostream>
#include<cstdlib>
#include<fstream>
#include<cstring>
using namespace std;

static char head_node[100],root_node[100],node[100];


model_detect_changes::model_detect_changes()
{
    vis.clear();
    //object_info.clear();
    object.clear();
}

model_detect_changes tmp;

static int get_node(void *NotUsed, int cnt, char **pValue, char **pName)//用来获得当前分支头节点的回调函数
{
    strcpy(root_node,pValue[0]);
    strcpy(head_node,pValue[1]);
    return 0;
}

static int update_node(void *NotUsed, int cnt, char **pValue, char **pName)//将节点更新为父亲节点的回调函数
{
    strcpy(node,pValue[0]);
    return 0;
}

static int get_object(void *NotUsed, int cnt, char **pValue, char **pName)//获得当前节点包含的object的回调函数
{
    if(tmp.vis[pValue[0]]) return 0;
    tmp.vis[pValue[0]]=1;
    if(atoi(pValue[1]) != -1) tmp.object.push_back(pValue[1]);
    return 0;
}

void model_detect_changes::detect_changes()
{

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    //从文件中读取当前分支
    ifstream file("current_branch.txt");
    int current_branch;
    file>>current_branch;
    file.close();

    //打开数据库
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if(rc != SQLITE_OK)
    {
        cerr<<"[ERROR]数据库打开失败："<<endl;
        exit(1);
    }
    else
    {
        clog<<"[INFO]数据库打开成功！"<<endl;
    }

    //获得当前分支头节点，即当前节点
    sprintf(sql, "SELECT BranchRoot,BranchHead FROM Branch WHERE ID='%d'",current_branch);
    rc = sqlite3_exec(db, sql, get_node, NULL, &zErrMsg);
    if(rc != SQLITE_OK)
    {
        cerr<<"[ERROR]节点信息获取失败："<<zErrMsg<<endl;
        exit(1);
    }
    else
    {
        clog<<"[INFO]节点信息获取成功！"<<endl;
    }


    strcpy(node,head_node);
    //节点不断向根节点更新从而获得所有包含的object，保存在tmp中
    while(!strcmp(node,root_node))
    {
        sprintf(sql,"SELECT File,Mode FROM Obj2Node WHERE Node='%s'",node);
        rc = sqlite3_exec(db, sql, get_object, NULL, &zErrMsg);
        if(rc != SQLITE_OK)
        {
            cerr<<"[ERROR]发生错误："<<zErrMsg<<endl;
            exit(1);
        }


        sprintf(sql, "SELECT Parent FROM Node WHERE SHA='%s'",node);
        rc = sqlite3_exec(db, sql, update_node, NULL, &zErrMsg);
        if(rc != SQLITE_OK)
        {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }

    //未完待续

}