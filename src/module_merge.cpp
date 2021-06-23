//
// Created by login on 2021/6/10.
//

#include "module_merge.h"
#include<iostream>
#include<sqlite3.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include"Database/database.h"
#include"Database/file_system.h"
#include"module_checkout.h"
using namespace std;

//按照originpath升序排列
bool compare_by_path(const file_info &a, const file_info &b) {
    return a.origin_path > b.origin_path;
}

struct module_merge_node_info {
    std::string SHA;
    std::string Parent;
    int Branch;
};

vector<module_merge_node_info> nd;

static int query_node_info_callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    //cout<<pValue[0]<<endl;
    if (cnt == 2) {
        module_merge_node_info tmp;
        tmp.SHA = pValue[0];
        tmp.Parent = pValue[1];
        nd.emplace_back(tmp);
        return 0;
    }
    return 0;

}

//获取节点的分支
static vector<int> branch_of_node;

static vector<string> branch_name;

static int query_node_s_branch_info_callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    branch_of_node.emplace_back(atoi(pValue[0]));
    branch_name.emplace_back(pValue[1]);
    return 0;
}


/**
     * 执行合并
     * @param node1 本分支的节点的sha1
     * @param node2 要被合并进来的节点的sha1
     * @param currentBranch 当前分支的编号
     */
void module_merge::merge(const std::string &node2) {

    char sql[1000];

    sprintf(sql, "SELECT SHA, Parent FROM Node WHERE SHA=(SELECT BranchHead From Branch WHERE ID=%d);",
    current_branch);
    //sprintf(sql, "SELECT BranchHead From Branch WHERE ID=%d",current_branch);
    //cout<<sql<<endl;

    rc = sqlite3_exec(db, sql, query_node_info_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]查询Node1信息失败: " << zErrMsg << endl;
        exit(1);
    }
    else if(nd.size()!=1)
    {
        cerr << "[ERROR]查询Node1信息失败, 找不到节点" << endl;
        exit(1);
    }



    sprintf(sql,
            "SELECT ID, Name FROM Branch WHERE ID=%d;",
            current_branch);
    rc = sqlite3_exec(db, sql, query_node_s_branch_info_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]查询当前分支HEAD节点信息失败: " << zErrMsg << endl;
        exit(1);
    }

    sprintf(sql, "SELECT SHA, Parent FROM Node WHERE SHA='%s';", node2.c_str());

    rc = sqlite3_exec(db, sql, query_node_info_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]查询Node信息失败: " << zErrMsg << endl;
        exit(1);
    } else if(nd.size()<2)
    {
        cerr << "[ERROR]查询Node信息失败, 找不到节点" << endl;
        exit(1);
    }

    sprintf(sql,
            "SELECT ID, Name FROM Branch WHERE ID=(SELECT Branch FROM Node2Branch WHERE Node='%s');",
            node2.c_str());
    rc = sqlite3_exec(db, sql, query_node_s_branch_info_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]查询Node信息失败: " << zErrMsg << endl;
        exit(1);
    }


    if (branch_of_node.size() < 2) {
        cerr << "[ERROR]Internal error" << endl;
        throw "Internal error.";
    }

    for(int i=1;i<branch_of_node.size();i++){
        if (branch_of_node[0] == branch_of_node[i]) {
            cerr << "Argument fault: 两个节点不能属于同一分支" << endl;
            throw "Argument fault: 两个节点不能属于同一分支";
        }
    }


    if (branch_of_node[0] != current_branch) {
        cerr << "[ERROR]基础节点不属于当前分支" << endl;
        throw "[ERROR]基础节点不属于当前分支";
    }

    module_detect_changes tmp_det;
    auto node_1_file_list = tmp_det.get_node_files(nd[0].SHA);

    auto node_2_file_list = tmp_det.get_node_files(nd[1].SHA);

    map<string, file_info> node1_file_list;

    //把当前节点的文件加入路径
    for (const auto &x: node_1_file_list) {
        node1_file_list[x.origin_path] = x;
    }

    vector<file_info> add_list;
    vector<file_info> change_list;

    //得到更改列表
    for (const auto &x: node_2_file_list) {
        //先前已经存在了这个原始路径
        if (node1_file_list.count(x.origin_path)) {
            //已经存在的文件
            auto pre = node1_file_list.find(x.origin_path);

            if (x.updated_datetime > pre->second.updated_datetime) {
                change_list.emplace_back(x);
            }
        } else
            //新增的
            add_list.emplace_back(x);
    }

    if (add_list.size() == 0 && change_list.size() == 0) {
        clog << "[INFO]无更改" << endl;
        exit(0);
    }
    //获得新节点的sha1
    stringstream tmp_SHA;


    //对原始路径排序，算节点的sha1
    vector<file_info> det;
    det.resize(change_list.size() + add_list.size());
    det.insert(det.end(), change_list.begin(), change_list.end());

    det.insert(det.end(), add_list.begin(), add_list.end());

    sort(det.begin(), det.end(), compare_by_path);

    for (auto &x:det) {
        tmp_SHA << x.origin_sha;
    }
    det.clear();

    //获取节点的sha1
    string new_node_sha1 = calculate_string_sha1(tmp_SHA.str());

    char tmp_time[500];
    auto tmpp = database::getCurrentTimeChar();

    strcpy(tmp_time, tmpp);
    free(tmpp);

    //创建新节点
    sprintf(sql,
            "INSERT INTO Node (SHA,CreatedDateTime,Parent,Message) VALUES ('%s','%s',(SELECT SHA FROM Node WHERE SHA='%s'),'%s' )",
            new_node_sha1.c_str(), tmp_time, nd[0].Parent.c_str(),
            ("Merge " + branch_name[0] + " into " + branch_name[1]).c_str());

    rc = sqlite3_exec(db, sql, 0, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]新节点创建失败：" << zErrMsg << endl;
        exit(1);
    } else {
        if(DEV_MODE)
            clog << "[INFO]新节点创建成功！" << endl;
    }

    //连接新节点和分支
    sprintf(sql,
            "INSERT INTO Node2Branch (ID,Node,Branch,CreatedDateTime) VALUES (NULL,(SELECT SHA FROM Node WHERE SHA='%s'),(SELECT ID FROM Branch WHERE ID='%d'),'%s')",
            new_node_sha1.c_str(), current_branch, tmp_time);
    rc = sqlite3_exec(db, sql, 0, NULL, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]新节点与分支连接失败：" << zErrMsg << endl;
        exit(1);
    } else {
        if(DEV_MODE)
            clog << "[INFO]新节点与分支连接成功！" << endl;
    }

    //将当前分支头节点改为新节点
    sprintf(sql, "UPDATE Branch SET BranchHead=(SELECT SHA FROM Node WHERE SHA='%s') WHERE ID='%d'",
            new_node_sha1.c_str(),
            current_branch);
    rc = sqlite3_exec(db, sql, 0, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]当前分支头节点更改失败：" << zErrMsg << endl;
        exit(1);
    } else {
        if(DEV_MODE)
            clog << "[INFO]当前分支头节点更改成功！" << endl;
    }

    //将add信息加入到连接表中
    for (auto &p:add_list) {

        sprintf(sql,
                "INSERT INTO Obj2Node (ID,File,Mode,Node,CreatedDateTime) VALUES (NULL,(SELECT CompressedSHA FROM Objects WHERE CompressedSHA='%s'),1,(SELECT SHA FROM Node WHERE SHA='%s'),'%s')",
                p.compressed_sha.c_str(), new_node_sha1.c_str(), tmp_time);
        rc = sqlite3_exec(db, sql, 0, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }
    //将change信息加入到连接表中
    for (auto &p:change_list) {

        sprintf(sql,
                "INSERT INTO Obj2Node (ID,File,Mode,Node,CreatedDateTime) VALUES (NULL,(SELECT CompressedSHA FROM Objects WHERE CompressedSHA='%s'),0,(SELECT SHA FROM Node WHERE SHA='%s'),'%s')",
                p.compressed_sha.c_str(), new_node_sha1.c_str(), tmp_time);
        rc = sqlite3_exec(db, sql, 0, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }

    clog << "[INFO]" + ("Merge " + node2 + " into " + branch_name[0]) << endl;

    //切换到新节点
    module_checkout tmp_checkout;

    tmp_checkout.checkout_switch_node(const_cast<char *>(new_node_sha1.c_str()));
    sqlite3_close(db);


}

