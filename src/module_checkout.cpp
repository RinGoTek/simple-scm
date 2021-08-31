//
// Created by windsoul on 2021/6/2.
//

#include <iostream>
#include <stdio.h>
#include <cstring>
#include <sqlite3.h>
#include <algorithm>
#include <fstream>
#include "headers/global.h"
#include "module_checkout.h"
#include "Database/file_system.h"
#include "module_detect_changes.h"
#include "Database/Compress.h"
#include "module_commit.h"
#include "module_new_branch.h"

using namespace std;

static vector<string> delete_list;
static vector<string> new_file_list;
static vector<string> old_file_list;
static vector<string> ignore_file_list;
static vector<string> ignore_dir_list;
static int node_exist_judge=0;
static int add_list_judge=0;
static int pID=0;
static char pNode[50];
static char UPtime[100];
static char root[]="000000";

static int node_is_exist(void *NotUsed, int cnt, char **pValue, char **pName)//获得分支的头节点的回调函数
{
    if(cnt>0) node_exist_judge=1;
    return 0;
}

static int add_list_is(void *NotUsed, int cnt, char **pValue, char **pName)//判断add_list是否为空
{
    if(cnt>0) add_list_judge=1;
    return 0;
}

static int get_branch_headnode(void *NotUsed, int cnt, char **pValue, char **pName)//获得分支的头节点
{
    //cout<<"xxxxx:"<<pValue[0]<<endl;
    strcpy(pNode,pValue[0]);
    if(cnt>0) node_exist_judge=1;
    return 0;
}

static int get_branch_id(void *NotUsed, int cnt, char **pValue, char **pName)//获得分支的头节点
{
    pID= atoi(pValue[0]);
    return 0;
}

static int get_update_time(void *NotUsed, int cnt, char **pValue, char **pName)//获得分支的头节点
{
    strcpy(UPtime,pValue[0]);
    return 0;
}

/*static int get_ignore_file(void *NotUsed, int cnt, char **pValue, char **pName)
{
    for(int i=0;i<cnt;i++){
        int k= (int)strlen(pValue[i])-1;
        if(pValue[i][k]=='/') {
            ignore_dir_list.push_back(pValue[i]);
            walk_return content= do_walk_folder(pValue[i]);

        }
            else ignore_file_list.push_back(pValue[i]);

    }
    return 0;
}*/

void module_checkout::checkout_switch_node(char *switch_node) {

    //检测是否有修改未提交
    ifstream fin(".simple-scm/HEAD");
    string HEAD;
    fin>>HEAD;
    fin.close();
    module_detect_changes op;
    detect_info x=op.detect_changes(HEAD);
    //puts("tess_line");
    if(x.change.size()>0||x.del.size()>0){
        cerr<<"[ERROR]您有更改尚未提交，是否切换分支?(y/n)"<<endl;
        cout<<">> ";
        char c;
        cin>>c;
        if(!(c=='y'||c=='Y')){
            cout<<"[INFO]操作已取消"<<endl;
            exit(0);
        }


    }


    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);

    if (rc) {
        cerr << "[ERROR]数据库加载失败！" << endl;
        exit(1);
    }

    char sql[500];
    //检测add_list是否为空
    sprintf(sql, "SELECT ID FROM AddList");
    rc = sqlite3_exec(db, sql, add_list_is ,NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]检测AddList表失败：" << zErrMsg << endl;
        exit(1);
    }
    if(add_list_judge==1){
        clog<<"AddList表存在未提交的文件！"<<endl;
        cout<<"若选择提交AddList表的文件后再切换分支请输入'YES'"<<endl<<"若选择清除AddList表后切换分支请输入'NO'"<<endl<<"若选择取消切换分支请输入'NOCK'..."<<endl;
        string s;
        cout<<">> ";
        cin>>s;
        transform(s.begin(),s.end(),s.begin(),::tolower);
        if(s=="nock"){
            clog<<"已取消切换分支!"<<endl;
            exit(1);
        }
        else if(s!=string("no")){
            sqlite3_close(db);
            cout<<"请输入提交的信息"<<endl;
            char* rbq_m;
            cin>>rbq_m;
            module_commit rbq;
            rbq.commit(rbq_m);
            rc = sqlite3_open(".simple-scm/simple-scm.db", &db);

            if (rc) {
                cerr << "[ERROR]数据库加载失败！" << endl;
                exit(1);
            }
        }else{
            //清空AddList表
            sprintf(sql, "DELETE FROM AddList");
            rc = sqlite3_exec(db, sql, NULL ,NULL, &zErrMsg);
            if (rc != SQLITE_OK) {
                cerr << "[ERROR]清空AddList表失败：" << zErrMsg << endl;
                exit(1);
            }
        }

    }


    //检测switch_node
    sprintf(sql,"SELECT * FROM Node WHERE SHA='%s'",switch_node);
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
    old_file_list= walk_folder(".");

    /*//获取不需要删除的文件
    sprintf(sql,"SELECT Path FROM IgnoreList");
    rc= sqlite3_exec(db,sql,get_ignore_file,NULL,&zErrMsg);
    if(rc!=SQLITE_OK){
        cerr<<"[ERROR]获取不需删除的文件失败:"<<zErrMsg<<endl;
        exit(0);
    }else{
        clog<<"获取不需删除的文件成功！"<<endl;
    };*/

    //获取需要删除的文件
    for(auto every_file:old_file_list)
    {
        /*auto it= find(ignore_file_list.begin(),ignore_file_list.end(),every_file);
        if(it==ignore_file_list.end())//如果在ignore_file_list中找不到则加入删除列表
        {*/
            delete_list.push_back(every_file);
       // }
    }

    //删除工作目录中的文件
    for(auto x:delete_list)
    {
        FileRemove(x.c_str());
    }

    //将该节点快照压缩文件解压到工作目录
    module_detect_changes rbq;
    vector<file_info> compressedSHA=rbq.get_node_files(switch_node);


    for(auto x:compressedSHA){
        Compress rbq1;
        //cout<<x.origin_path<<endl;
        sprintf(sql, "SELECT UpdatedDateTime FROM Objects WHERE CompressedSHA='%s'", x.compressed_sha.c_str());

        rc = sqlite3_exec(db, sql, get_update_time ,NULL, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }

        rbq1.decompress(x.compressed_path,x.origin_path,UPtime);
    }
    clog<<"[INFO]压缩文件解压到工作目录成功!"<<endl;

    sqlite3_close(db);

    ofstream fout(".simple-scm/HEAD");
    fout<<switch_node;
    fout.close();
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
    //cout<<switch_branch<<endl;
    sprintf(sql,"SELECT BranchHead FROM Branch WHERE NAME='%s';",switch_branch);
    //cout<<sql<<endl;
    rc= sqlite3_exec(db,sql,get_branch_headnode,NULL,&zErrMsg);
    if(rc!=SQLITE_OK){
        cerr<<"[ERROR]获取所切换分支的头节点失败:"<<zErrMsg<<endl;
        exit(0);
    }
    if(node_exist_judge==0){//分支不存在
        cerr<<"[ERROR]所切换的分支不存在!"<<endl;
        /*
        module_new_branch rbq;
        rbq.create_branch(switch_branch);
         */
        exit(0);
    }

    sprintf(sql,"SELECT ID FROM Branch WHERE NAME='%s';",switch_branch);
    //cout<<sql<<endl;
    rc= sqlite3_exec(db,sql,get_branch_id,NULL,&zErrMsg);
    if(rc!=SQLITE_OK){
        cerr<<"[ERROR]获取所切换分支的ID失败:"<<zErrMsg<<endl;
        exit(0);
    }


    sqlite3_close(db);

    //cout<<pNode<<endl;
    this->checkout_switch_node(pNode);
    cerr<<"[INFO]切换分支成功！"<<endl;

    ofstream cou(".simple-scm/current_branch.txt");
    cou<<pID;
    cou.close();
}

void module_checkout::help() {

}
