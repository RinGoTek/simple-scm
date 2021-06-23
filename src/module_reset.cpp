//
// Created by windsoul on 2021/6/11.
//
#include "module_reset.h"
#include "module_checkout.h"
#include <iostream>
#include <sqlite3.h>
#include "fstream"
#include "headers/global.h"
using namespace  std;
static int to_node_branch_id;

static int current_branch;
static bool is_current_branch_flag = false;
static int callback(void *NotUsed, int cnt, char **pValue, char **pName)//用于获得branch表所有信息的回调函数
{
    to_node_branch_id = atoi(pValue[0]);
    if(atoi(pValue[0]) == current_branch)
        is_current_branch_flag=true;
    return 0;
}

void module_reset::reset(char *To_Node)
{


    ofstream cou(".simple-scm/HEAD");
    cou<<To_Node;
    cou.close();

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if (rc) {
        cerr << "[ERROR]数据库加载失败！" << endl;
        exit(1);
    }

    ifstream file(".simple-scm/current_branch.txt");

    file >> current_branch;
    file.close();

    char sql[500];
    sprintf(sql,"SELECT Branch FROM Node2Branch WHERE Node='%s'",To_Node);
    rc= sqlite3_exec(db,sql,callback,NULL,&zErrMsg);
    if(rc!=SQLITE_OK){
        cerr << "[ERROR]查询节点信息失败:" <<zErrMsg<< endl;
        exit(1);
    }

    if(!is_current_branch_flag)
    {
        cerr<<"[ERROR]目标节点不属于当前分支！"<<endl;
        exit(1);
    }

    sprintf(sql,"UPDATE Branch SET BranchHead ='%s' WHERE ID =%d",To_Node,current_branch);
    rc= sqlite3_exec(db,sql,NULL,NULL,&zErrMsg);
    if(rc!=SQLITE_OK){
        cerr << "[ERROR]更新Branch表失败:" <<zErrMsg<< endl;
        exit(0);
    }

    module_checkout rbq;
    rbq.checkout_switch_node(To_Node);
    clog<<"[INFO]回退版本成功！"<<endl;
}
