//
// Created by login on 2021/6/10.
//

#include "module_merge.h"
#include<iostream>
#include<sqlite3.h>
#include <fstream>
#include"Database/database.h"
#include"Database/file_system.h"

using namespace std;

struct node_info
{
    std::string SHA;
    std::string Parent;
    int Branch;
};

vector<node_info>nd;

static int query_node_info_callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    if(cnt == 5)
    {
        node_info tmp;
        tmp.SHA = pValue[0];
        tmp.Parent = pValue[1];
        nd.emplace_back(tmp);
        return 0;
    }
    return 1;

}

//获取节点的分支
static vector<int> branch_of_node;
static int query_node_s_branch_info_callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    branch_of_node.emplace_back(atoi(pValue[0])) ;
    return 0;
}


/**
     * 执行合并
     * @param node1 本分支的节点的sha1
     * @param node2 要被合并进来的节点的sha1
     * @param currentBranch 当前分支的编号
     */
void module_merge::merge(const std::string& node2) {

    char sql[1000];

    sprintf(sql, "SELECT (SHA, Parent) FROM Node WHERE SHA='(SELECT BranchHead From Branch WHERE ID=%d)';", current_branch);

    rc = sqlite3_exec(db, sql, query_node_info_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]查询Node1信息失败: " << zErrMsg << endl;
    } else {
        //clog << "[INFO]根节点创建成功！" << endl;
    }

    sprintf(sql, "SELECT (ID) FROM Branch WHERE ID=(SELECT Branch FROM Node2Branch WHERE Node='(SELECT BranchHead From Branch WHERE ID=%d)');", current_branch);
    rc = sqlite3_exec(db, sql,query_node_s_branch_info_callback, 0, &zErrMsg);

    if(rc != SQLITE_OK)
    {
        cerr << "[ERROR]查询当前分支HEAD节点信息失败: " << zErrMsg << endl;
    }

    sprintf(sql, "SELECT (SHA, Parent) FROM Node WHERE SHA='%s';", node2.c_str());

    rc = sqlite3_exec(db, sql, query_node_info_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]查询Node信息失败: " << zErrMsg << endl;
    } else {
        //clog << "[INFO]根节点创建成功！" << endl;
    }

    sprintf(sql, "SELECT (ID) FROM Branch WHERE ID=(SELECT Branch FROM Node2Branch WHERE Node='(SELECT SHA FROM Node WHERE SHA='%s')');", node2.c_str());
    rc = sqlite3_exec(db, sql,query_node_s_branch_info_callback, 0, &zErrMsg);

    if(rc != SQLITE_OK)
    {
        cerr << "[ERROR]查询Node信息失败: " << zErrMsg << endl;
    }



    if(branch_of_node.size()!=2)
    {
        cerr<<"[ERROR]Internal error"<<endl;
        throw "Internal error.";
    }

    if(branch_of_node[0]==branch_of_node[1])
    {
        cerr<<"Argument fault: 两个节点不能属于同一分支"<<endl;
        throw "Argument fault: 两个节点不能属于同一分支";
    }

    if(branch_of_node[0]!=current_branch)
    {
        cerr<<"[ERROR]基础节点不属于当前分支"<<endl;
        throw "[ERROR]基础节点不属于当前分支";
    }






}

