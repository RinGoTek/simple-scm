//
// Created by a2447742618 on 2021/5/28.
//

#include "module_detect_changes.h"
#include"Database/file_system.h"
#include"Database/database.h"
#include"headers/global.h"
#include<sqlite3.h>
#include<iostream>
#include<cstdlib>
#include<fstream>
#include<cstring>
#include<stack>
#include<vector>
#include<algorithm>
#include<time.h>

using namespace std;

static char head_node[500], node[500];
static map<string, bool> vis;
static vector<string> object_sha;
static vector<string> object_path;
static stack<string> walk_list;
static vector<string> ignore_object;
static vector<file_info> node_info_sav;
static vector<string> add;

void init() {
    object_sha.clear();
    object_path.clear();
    vis.clear();
    add.clear();
    ignore_object.clear();
    node_info_sav.clear();
}


static int get_add_path(void *NotUsed, int cnt, char **pValue, char **pName) {
    add.emplace_back(pValue[0]);
    return 0;
}

static int get_node(void *NotUsed, int cnt, char **pValue, char **pName)//用来获得当前分支头节点的回调函数
{
    strcpy(head_node, pValue[0]);
    return 0;
}

static int update_node(void *NotUsed, int cnt, char **pValue, char **pName)//将节点更新为父亲节点的回调函数
{
    strcpy(node, pValue[0]);
    return 0;
}

static string origin_path_tmp;

static int get_origin_path(void *NotUsed, int cnt, char **pValue, char **pName) {
    origin_path_tmp = pValue[0];
    return 0;
}

static int get_object(void *NotUsed, int cnt, char **pValue, char **pName)//获得当前节点包含的object的回调函数
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[500];

    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据库打开失败：" << endl;
        exit(1);
    }

    sprintf(sql, "SELECT OriginPath FROM Objects WHERE CompressedSHA='%s'", pValue[0]);
    rc = sqlite3_exec(db, sql, get_origin_path, NULL, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    sqlite3_close(db);

    if (vis[origin_path_tmp]) return 0;
    vis[origin_path_tmp] = 1;
    if (atoi(pValue[1]) != -1) object_sha.push_back(pValue[0]);
    return 0;
}

static int select_ignore_callback(void *NotUsed, int cnt, char **pValue, char **pName)//获取ignore信息的函数
{
    //把信息添加到ignore数组或者等待遍历的目录stack
    if (is_file(pValue[0])) ignore_object.emplace_back(pValue[0]);
    else if (is_dir(pValue[0])) walk_list.push(pValue[0]);
    return 0;
}

static int get_object_info(void *NotUsed, int cnt, char **pValue, char **pName)//获得文件所有信息
{
    file_info tmp;
    tmp.compressed_sha = pValue[0];
    tmp.origin_sha = pValue[1];
    tmp.origin_path = pValue[2];
    tmp.compressed_path = pValue[3];
    tmp.created_datetime = pValue[4];
    tmp.updated_datetime = pValue[5];
    node_info_sav.emplace_back(tmp);
    return 0;
}

detect_info module_detect_changes::detect_changes(string NodeSHA) {

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[500];
    init();

    clock_t start_time;
    if(DEV_MODE) start_time = clock();

    /*
    //从文件中读取当前分支
    ifstream file(".simple-scm/current_branch.txt");
    int current_branch;
    file >> current_branch;
    file.close();
     */

    //打开数据库
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据库打开失败：" << endl;
        exit(1);
    }

    /*
    //获得当前分支头节点，即当前节点
    sprintf(sql, "SELECT BranchHead FROM Branch WHERE ID='%d'", current_branch);
    rc = sqlite3_exec(db, sql, get_node, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]节点信息获取失败：" << zErrMsg << endl;
        exit(1);
    } else {
        clog << "[INFO]节点信息获取成功！" << endl;
    }

    strcpy(head_node, NodeSHA.c_str());
    strcpy(node, head_node);


    //节点不断向根节点更新从而获得所有包含的object(CompressedSHA)
    while (strcmp(node, "000000")) {

        sprintf(sql, "SELECT File,Mode FROM Obj2Node WHERE Node='%s'", node);

        rc = sqlite3_exec(db, sql, get_object, NULL, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }

        sprintf(sql, "SELECT Parent FROM Node WHERE SHA='%s'", node);
        rc = sqlite3_exec(db, sql, update_node, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }
     */

    detect_info sav;

    vector<file_info>node_info= get_node_files(NodeSHA);

    for(auto p:node_info) sav.path2SHA[p.origin_path]=p.compressed_sha;

    //找到所有ignore的文件或文件夹(origin路径)
    sprintf(sql, "SELECT Path From IgnoreList");
    rc = sqlite3_exec(db, sql, select_ignore_callback, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    //获得所有ignore文件夹下的文件(origin路径)
    while (!walk_list.empty()) {
        string current_dir = walk_list.top();
        walk_list.pop();

        vector<string> tmp = walk_folder(current_dir);
        ignore_object.insert(ignore_object.end(), tmp.begin(), tmp.end());
    }

    //获得add表中的信息
    sprintf(sql, "SELECT OriginPath FROM AddList");
    rc = sqlite3_exec(db, sql, get_add_path, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    /*
    //根据节点包含的文件CompressedSHA获得相应的路径
    for (auto &p:object_sha) {
        sprintf(sql, "SELECT OriginPath,CompressedSHA FROM Objects WHERE CompressedSHA ='%s'", p.c_str());
        rc = sqlite3_exec(db, sql, get_object_path, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }
     */

    auto local_object = walk_folder(".");
    //先获得本地所有文件路径，通过排除法得到还没有add的文件路径

    //遍历节点所包含文件的路径，获得检测信息(origin路径)
    for (auto &p:node_info) {

        auto it = find(ignore_object.begin(), ignore_object.end(), p.origin_path);

        //在忽略列表中
        if (it != ignore_object.end()) continue;

        struct stat buf;
        if (stat(p.origin_path.c_str(), &buf) == -1)//找不到该文件
        {
            sav.del.emplace_back(p.origin_path);
            continue;
        }

        //已经存在于节点中的文件，从not add移除
        for (auto it = local_object.begin(); it < local_object.end(); it++) {
            if (*it == p.origin_path) {
                local_object.erase(it);
                break;
            }
        }

        if (strcmp(p.updated_datetime.c_str(), database::getTimeChar(buf.st_mtime))) sav.change.emplace_back(p.origin_path);
        //比较文件更新时间判断文件是否发生更改
    }

    for (auto it = local_object.begin(); it < local_object.end();) {
        auto q = find(add.begin(), add.end(), *it);

        //已经存在AddList表的也从not add移除
        if (q != add.end()) it=local_object.erase(it);//erase后原迭代器失效，it自动后移
        else it++;
    }

    sav.not_add = local_object;
    //for (auto &p:sav.not_add) clog << "not add " << p << endl;

    sqlite3_close(db);
    clog << "[INFO]检测完成" << endl;
    cout << "detect changes完成！耗时：" << 1.0 * (clock() - start_time) / 1000.0 << "ms" << endl;
    return sav;
}

vector<file_info> module_detect_changes::get_node_files(string NodeSHA) {
    init();

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[500];
    init();

    //打开数据库
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据库打开失败：" << endl;
        exit(1);
    }

    strcpy(node, NodeSHA.c_str());

    while (strcmp(node, "000000")) {

        sprintf(sql, "SELECT File,Mode FROM Obj2Node WHERE Node='%s'", node);

        rc = sqlite3_exec(db, sql, get_object, NULL, &zErrMsg);

        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }

        sprintf(sql, "SELECT Parent FROM Node WHERE SHA='%s'", node);
        rc = sqlite3_exec(db, sql, update_node, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }

    for (auto p:object_sha) {
        sprintf(sql, "SELECT * FROM Objects WHERE CompressedSHA='%s'", p.c_str());

        rc = sqlite3_exec(db, sql, get_object_info, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }
    sqlite3_close(db);

    return node_info_sav;
}
