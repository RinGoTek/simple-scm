//
// Created by login on 2021/8/31.
//

#include "module_edit.h"
#include<iostream>
#include<vector>
#include<map>
#include<sqlite3.h>

using namespace std;
static pair<int, string> is_node_editable = make_pair(-1, "");

static int callback_get_node_editable(void *NotUsed, int cnt, char **pValue, char **pName)//用于获得node是否可编辑的回调函数
{
    string editable = pValue[0];
    is_node_editable.first = atoi(editable.c_str());
    is_node_editable.second = pValue[1];
    return 0;
}

void module_edit::edit(char *parameters[], int size) {
    if (size == 1 && parameters[0] == "help" || (size == 0)) {
        help();
    }
    ops = parse_ops(parameters, size);

    for (const auto &x:ops) {
        if (x.first == "-n")
            mp["node"] = x.second;
        else if (x.first == "-t")
            mp["text"] = x.second;
    }
    if (mp.count("node") && mp.count("text") && mp.size() == 2) {
        //编辑节点提交文本
        edit_node_text();
    }


}

void module_edit::help() {
    cout << "以下是edit的帮助信息\n"
         << "edit -n <SHA1> -t <TEXT>   编辑提交文本\n";
    exit(0);
}

static void print_blank(int len)
{
    for(int i=0;i<len;++i)
        cout<<" ";
}

void module_edit::edit_node_text() {
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
        if (DEV_MODE)
            clog << "[INFO]数据库打开成功！" << endl;
    }

    //获得节点是否可编辑
    sprintf(sql, "SELECT Editable, Message FROM Node WHERE SHA='%s'", mp["node"].c_str());
    rc = sqlite3_exec(db, sql, callback_get_node_editable, NULL, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }
    if (is_node_editable.first == -1) {
        clog << "[ERROR]指定节点不存在！" << endl;
        exit(0);
    } else if (is_node_editable.first == 0) {
        clog << "[ERROR]指定节点不可被编辑！" << endl;
        exit(0);
    }

    //更新节点信息
    sprintf(sql, "UPDATE Node SET Message = '%s' WHERE SHA='%s'", mp["text"].c_str(), mp["node"].c_str());
    rc = sqlite3_exec(db, sql, 0, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }
    cout << "更新节点信息成功：\n";
    int blank_last = (is_node_editable.second.length() - 1) / 2;
    int blank_new = (mp["text"].length() - 1) / 2;
    string arrow = "  -->  ";
    print_blank(blank_last);
    cout << "旧";
    print_blank(blank_last+arrow.length());
    print_blank(blank_new);
    cout << "新"<<endl;


    cout << is_node_editable.second << arrow << mp["text"] << endl;
}
