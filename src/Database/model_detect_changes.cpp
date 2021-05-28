//
// Created by a2447742618 on 2021/5/28.
//

#include "model_detect_changes.h"
#include<sqlite3.h>
#include<iostream>
#include<cstdlib>
#include<fstream>
using namespace std;

string head_node,root_node,node;


model_detect_changes::model_detect_changes()
{
    vis.clear();
    object_info.clear();
    object.clear();
}
model_detect_changes tmp;

static int get_node(void *NotUsed, int cnt, char **pValue, char **pName)
{
    head_node=pValue[0];
    root_node=pValue[1];
    return 0;
}

static int update_node(void *NotUsed, int cnt, char **pValue, char **pName)
{
    node=pValue[0];
    return 0;
}

static int get_object(void *NotUsed, int cnt, char **pValue, char **pName)
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

    ifstream file("current_branch.txt");
    int current_branch;
    file>>current_branch;
    clog<<current_branch<<endl;

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

    sql = "SELECT BranchRoot,BranchHead FROM Branch WHERE ID=a";
    rc = sqlite3_exec(db, sql, get_node, NULL, &zErrMsg);
    if(rc != SQLITE_OK)
    {
        clog<<"[ERROR]节点信息获取失败："<<zErrMsg<<endl;
    }
    else
    {
        clog<<"[INFO]节点信息获取成功！"<<endl;
    }

    node=head_node;
    while(node != root_node)
    {
        sql = "SELECT File,Mode FROM Obj2Node WHERE Node=node";
        rc = sqlite3_exec(db, sql, get_object, NULL, &zErrMsg);
        if(rc != SQLITE_OK)
        {
            clog<<"[ERROR]发生错误："<<zErrMsg<<endl;
            exit(1);
        }


        sql = "SELECT Parent FROM Node WHERE SHA=node";
        rc = sqlite3_exec(db, sql, update_node, NULL, &zErrMsg);
        if(rc != SQLITE_OK)
        {
            clog << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }
    //cout<<tmp.object.size()<<endl;

}