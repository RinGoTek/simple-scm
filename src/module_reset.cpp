//
// Created by windsoul on 2021/6/11.
//
#include "module_reset.h"
#include "module_checkout.h"
#include <iostream>
#include <sqlite3.h>
#include "fstream"
using namespace  std;

void module_reset::reset(char *To_Node)
{
    module_checkout rbq;
    rbq.checkout_switch_node(To_Node);

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
    int current_branch;
    file >> current_branch;
    file.close();

    char sql[500];

    sprintf(sql,"UPDATE Branch SET BranchHead ='%s' WHERE ID =%d",To_Node,current_branch);
    rc= sqlite3_exec(db,sql,NULL,NULL,&zErrMsg);
    if(rc!=SQLITE_OK){
        cerr << "[ERROR]更新Branch表失败:" <<zErrMsg<< endl;
        exit(0);
    }

    clog<<"[INFO]回退版本成功！"<<endl;
}
