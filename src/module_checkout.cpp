//
// Created by windsoul on 2021/6/2.
//

/*#include "module_checkout.h"
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
}*/

#include <iostream>
#include <stdio.h>
#include <cstring>
#include <sqlite3.h>
#include <algorithm>
#include "module_checkout.h"
#include "Database/file_system.h"
#include "module_detect_changes.h"

using namespace std;

static vector<string> delete_list;
static vector<string> compressedSHA;
static vector<string> new_file_list;
static vector<string> old_file_list;
static vector<string> ignore_list;
static char switch_branch_headnode[50];

static int FileRemove(const char* filename)//const char*,删除文件
{
    return remove(filename);
}//删除成功返回值为0，删除失败返回值为-1

static void CopyFile(const char* sourcefile, char* new_file)//将路径sourcefile的文件在路径new_file处复制一份
{
    string command = "cp ";
    command+= sourcefile;
    command+= " ";
    command+= new_file;
    system((char*)command.c_str());
}

static int get_headnode(void *NotUsed, int cnt, char **pValue, char **pName)//获得分支的头节点的回调函数
{
    strcpy(switch_branch_headnode,pValue[0]);
    return 0;
}

static int get_object_path(void *NotUsed, int cnt, char **pValue, char **pName)//获得该分支最新版本的文件的压缩SHA
{
    for(int i=0;i<cnt;i++){
        compressedSHA.push_back(pValue[i]);
    }
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

void module_checkout::checkout_switch_branch(char *switch_branch) {
    /*int ok= detect_info()
    if (ok==0)
    {
        cerr<<"[ERROR]请将做出的修改进行提交或删除后再切换分支"<<endl;
        exit(0);
    }*/

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

        //获取所切换分支的头节点
        sprintf(sql,"SELECT BranchHead FROM Branch WHERE NAME='%s'",switch_branch);
        rc= sqlite3_exec(db,sql,get_headnode,NULL,&zErrMsg);
        if(rc!=SQLITE_OK){
            cerr<<"[ERROR]获取所切换分支的头节点失败:"<<zErrMsg<<endl;
            exit(0);
        }else{
            clog<<"[INFO]获取所切换分支的头节点成功！"<<endl;
        }

        //获取所切换分支的文件的压缩SHA
        sprintf(sql,"SELECT FILE FROM Obj2Node WHERE NODE='%s'",switch_branch_headnode);
        rc= sqlite3_exec(db,sql,get_object_path,NULL,&zErrMsg);
        if(rc!=SQLITE_OK){
            cerr<<"[ERROR]获取所切换分支的文件的压缩SHA失败:"<<zErrMsg<<endl;
            exit(0);
        }else{
            clog<<"[INFO]获取所切换分支的文件的压缩SHA成功!"<<endl;
        }

        //获取所切换分支的文件的路径,
        for(auto x:compressedSHA) {
            sprintf(sql,"SELECT OriginPath FROM Objects WHERE CompressedSHA='%s'",x.c_str());
            rc = sqlite3_exec(db, sql, get_new_file_path, NULL,&zErrMsg);
            if(rc!=SQLITE_OK){
                cerr<<"[ERROR]获取所切换分支的文件的路径失败:"<<zErrMsg<<endl;
                exit(0);
            }
        }
        clog<<"[INFO]获取所切换分支的文件的路径成功!"<<endl;

        //获取工作目录的文件
        old_file_list= walk_folder(".simple-scm");

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
                delete_list.push_back(every_file);
        }

        //删除工作目录中的文件
        for(auto x:delete_list)
        {
            FileRemove(x.c_str());
        }

        //将所切换的分支的文件复制到工作目录中
        for(auto x:new_file_list)
        {
            CopyFile(x.c_str(),".simple-scm");
        }
}

