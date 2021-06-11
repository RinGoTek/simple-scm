//
// Created by windsoul on 2021/6/2.
//

#include <iostream>
#include <stdio.h>
#include <cstring>
#include <sqlite3.h>
#include <algorithm>
#include "headers/cache.h"
#include "module_checkout.h"
#include "Database/file_system.h"
#include "module_detect_changes.h"
#include "Database/Compress.h"
#include "module_new_branch.h"

using namespace std;

static vector<string> delete_list;
static vector<string> new_file_list;
static vector<string> old_file_list;
static vector<string> ignore_list;
static int node_exist_judge=0;
static char pNode[50];
static char root[]="000000";

static int node_is_exist(void *NotUsed, int cnt, char **pValue, char **pName)//获得分支的头节点的回调函数
{
    if(cnt>0) node_exist_judge=1;
    return 0;
}

static int get_node_parent(void *NotUsed, int cnt, char **pValue, char **pName)
{
    strcpy(pNode,pValue[0]);
    return 0;
}


static int get_new_file_path(void *NotUsed, int cnt, char **pValue, char **pName)//获取所切换分支的文件的路径
{
    new_file_list.push_back(pValue[0]);
    return 0;
}

static int get_ignore_file(void *NotUsed, int cnt, char **pValue, char **pName)
{
    for(int i=0;i<cnt;i++){
        ignore_list.push_back(pValue[i]);
    }
    return 0;
}

void module_checkout::checkout_switch_node(char *switch_node) {

    //检测是否有修改未提交
    module_detect_changes op;
    detect_info x=op.detect_changes();
    if(x.change.size()||x.del.size()){
        cerr<<"[ERROR]请将做出的修改进行提交或删除后再切换节点"<<endl;
        exit(0);
    }

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);

    if (rc) {
        cerr << "[ERROR]数据库加载失败！" << endl;
        exit(1);
    } else {
        clog << "[INFO]数据库加载成功！" << endl;
    }

    char sql[500];

    //检测switch_node

    sprintf(sql,"SELECT FROM Node WHERE SHA='%s'",switch_node);
    rc= sqlite3_exec(db,sql,node_is_exist,NULL,&zErrMsg);
    if(rc!=SQLITE_OK){
        cerr << "[ERROR]检测所切换的节点失败:" <<zErrMsg<< endl;
        exit(0);
    }
    if(node_exist_judge==0){
        cerr<<"[ERROR]所切换的节点不存在!"<<endl;
        exit(0);
    }

    //获取工作目录的文件
    old_file_list= walk_folder(cwd);

    //获取不需要删除的文件
    sprintf(sql,"SELECT Path FROM IgnoreList");
    rc= sqlite3_exec(db,sql,get_ignore_file,NULL,&zErrMsg);
    if(rc!=SQLITE_OK){
        cerr<<"[ERROR]获取不需删除的文件失败:"<<zErrMsg<<endl;
        exit(0);
    }else{
        clog<<"获取不需删除的文件成功！"<<endl;
    };

    //获取需要删除的文件
    for(auto every_file:old_file_list)
    {
        auto it= find(ignore_list.begin(),ignore_list.end(),every_file);
        if(it==ignore_list.end())//如果在ignore_list中找不到则加入删除列表
        {
            delete_list.push_back(every_file);
        }
    }

    //删除工作目录中的文件
    for(auto x:delete_list)
    {
        FileRemove(x.c_str());
    }

    //将该节点快照压缩文件解压到工作目录
    module_detect_changes rbq;
    vector<node_info> compressedSHA=rbq.get_node_info(switch_node);
    for(auto x:compressedSHA){
        Compress rbq1;
        rbq1.decompress(x.compressed_path,cwd);
    }
    clog<<"[INFO]压缩文件解压到工作目录成功!"<<endl;

    sqlite3_close(db);
}

void module_checkout::checkout_switch_branch(char *switch_branch)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);

    if (rc) {
        cerr << "[ERROR]数据库加载失败！" << endl;
        exit(1);
    }

    char sql[500];

    //获取所切换分支的头节点
    sprintf(sql,"SELECT BranchHead FROM Branch WHERE NAME='%s'",switch_branch);
    rc= sqlite3_exec(db,sql,node_is_exist,NULL,&zErrMsg);
    if(rc!=SQLITE_OK){
        cerr<<"[ERROR]获取所切换分支的头节点失败:"<<zErrMsg<<endl;
        exit(0);
    }
    if(node_exist_judge==0){//分支不存在则新建该分支
        cerr<<"[ERROR]所切换的分支不存在!"<<endl;
        module_new_branch rbq;
        rbq.create_branch(switch_branch);
        exit(0);
    }
    module_checkout op;
    op.checkout_switch_node(pNode);
}