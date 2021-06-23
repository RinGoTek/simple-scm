//
// Created by a2447742618 on 2021/6/1.
//

#include "module_add.h"
#include"Database/file_system.h"
#include"Database/database.h"
#include<sys/stat.h>
#include<sqlite3.h>
#include<vector>
#include<stack>
#include<algorithm>
#include<fstream>
#include<cstring>
#include<sstream>

using namespace std;

static int callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    for (int i = 0; i < cnt; i++) cout << pName[i] << "=" << (pValue[i] ? pValue[i] : "NULL") << endl;
    return 0;
}

static bool file_exit;

static int check_exit(void *NotUsed, int cnt, char **pValue, char **pName) {
    file_exit = 1;
    return 0;
}


static vector<string> ignore;
static stack<string> walk_list;
static vector<string> file;
static vector<string> origin_path_of_node;
static char head_node[500], node[500];
static string origin_path_tmp;

static int update_node(void *NotUsed, int cnt, char **pValue, char **pName)//将节点更新为父亲节点的回调函数
{
    strcpy(node, pValue[0]);
    return 0;
}

static int get_origin_path(void *NotUsed, int cnt, char **pValue, char **pName) {
    origin_path_tmp = pValue[0];
    return 0;
}

static int get_object(void *NotUsed, int cnt, char **pValue, char **pName)//获得当前节点包含的文件的origin_path
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

    origin_path_of_node.push_back(origin_path_tmp);
    return 0;
}

static int select_ignore_callback(void *NotUsed, int cnt, char **pValue, char **pName)//获取ignore信息的函数
{
    //把信息添加到ignore数组或者等待遍历的目录stack
    if (is_file(pValue[0])) ignore.emplace_back(pValue[0]);
    else if (is_dir(pValue[0])) walk_list.push(pValue[0]);
    return 0;
}

void module_add::add(char *path) {
    ifstream fin(".simple-scm/HEAD");
    string HEAD;
    fin >> HEAD;
    fin.close();
    strcpy(head_node, HEAD.c_str());

    int cnt = 0;

    struct stat buf;
    if (stat(path, &buf) == -1)//找不到该文件
    {
        clog << "[ERROR]找不到该文件！" << endl;
        exit(1);
    }

    if (is_file(path)) file.emplace_back(path);
    else if (is_dir(path)) file = walk_folder(path);
    else return;

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[500];

    //打开数据库
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据库打开失败：" << zErrMsg << endl;
        exit(1);
    }

    //找到所有ignore的文件或文件夹
    sprintf(sql, "SELECT Path From IgnoreList");
    rc = sqlite3_exec(db, sql, select_ignore_callback, nullptr, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    //找到ignore文件夹下所有文件
    while (!walk_list.empty()) {
        string current_dir = walk_list.top();
        walk_list.pop();

        vector<string> tmp = walk_folder(current_dir);
        ignore.insert(ignore.end(), tmp.begin(), tmp.end());
    }

    strcpy(node, head_node);
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

    //把不在ignore中的所有文件信息加入AddList中
    for (auto &p:file) {
        auto it = find(ignore.begin(), ignore.end(), p);

        //在忽略列表中
        if (it != ignore.end()) continue;

        file_exit = 0;

        memset(sql, 0, sizeof(char)*500);
        sprintf(sql, "SELECT * FROM AddList WHERE OriginPath='%s'", p.c_str());
        rc = sqlite3_exec(db, sql, check_exit, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "发生错误: " << zErrMsg << endl;
            exit(1);
        }

        if (file_exit) {
            clog << "[ERROR]请勿重复添加文件" << p << endl;
            continue;
        }

        auto it2 = find(origin_path_of_node.begin(), origin_path_of_node.end(), p);

        if (it2 != origin_path_of_node.end()) {
            if(DEV_MODE)
                cerr << "[ERROR]" << p << "路径已存在！" << endl;
            continue;
        }

        struct stat buf;
        stat(p.c_str(), &buf);

        //memset(sql, 0, sizeof(char)*500);

        sprintf(sql,"INSERT INTO AddList (OriginSHA,OriginPath,CreatedDateTime,UpdatedDateTime) VALUES ('%s','%s','%s','%s')",calculate_sha1(p),p.c_str(),database::getTimeChar(buf.st_ctime),database::getTimeChar(buf.st_mtime));
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            //cout<<"sha"<<calculate_sha1(p)<<endl;
            //cout<<sql_<<endl;
            cerr << "发生错误: " << zErrMsg << endl;
            exit(1);
        }
        cnt++;

    }

    clog << "[INFO]添加成功，共有" << cnt << "条添加记录" << endl;
    sqlite3_close(db);

}