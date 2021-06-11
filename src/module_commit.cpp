//
// Created by a2447742618 on 2021/5/28.
//

#include"Database/database.h"
#include"Database/file_system.h"
#include "module_commit.h"
#include "module_detect_changes.h"
#include"Database/Compress.h"
#include<iostream>
#include<fstream>
#include<sqlite3.h>
#include<cstring>
#include<vector>
#include<sstream>
#include<string>
#include<algorithm>


using namespace std;


static string new_sha1;
static vector<string> add;
static string branch_head;


static int callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    for (int i = 0; i < cnt; i++) cout << pName[i] << "=" << (pValue[i] ? pValue[i] : "NULL") << endl;
    return 0;
}

static int get_add_path(void *NotUsed, int cnt, char **pValue, char **pName) {
    add.emplace_back(pValue[0]);
    return 0;
}

static int get_branch_head(void *NotUsed, int cnt, char **pValue, char **pName) {
    branch_head = pValue[0];
    return 0;
}

void module_commit::commit(char *Message) {

    clog << "[INFO]正在检测更改信息..." << endl;
    add.clear();

    detect_info info;
    module_detect_changes commit_tmp;


    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[1000];

    //打开数据库
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据库打开失败" << endl;
        exit(1);
    }

    //从文件中读取当前分支
    ifstream file(".simple-scm/current_branch.txt");
    int current_branch;
    file >> current_branch;
    file.close();

    sprintf(sql, "SELECT BranchHead FROM Branch WHERE ID=%d", current_branch);
    rc = sqlite3_exec(db, sql, get_branch_head, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    info = commit_tmp.detect_changes(branch_head);

    //获得add表中的信息
    sprintf(sql, "SELECT OriginPath FROM AddList");
    rc = sqlite3_exec(db, sql, get_add_path, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    if (info.del.empty() && info.change.empty() && add.empty()) {
        cerr << "[ERROR]没有更改信息！" << endl;
        exit(1);
    }

    //清空AddList表
    sprintf(sql, "DELETE FROM AddList");
    rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]清空AddList表失败：" << zErrMsg << endl;
        exit(1);
    } else {
        clog << "[INFO]AddList表清空成功！" << endl;
    }

    //获得新节点的sha1
    stringstream tmp_SHA;
    for (auto &p:info.change) clog << "change  " << p << endl;
    for (auto &p:info.del) clog << "del  " << p << endl;
    for (auto &p:add) clog << "add  " << p << endl;

    //对原始路径排序，算节点的sha1
    vector<string> det;
    det.resize(info.change.size() + info.del.size() + add.size());
    det.insert(det.end(), info.change.begin(), info.change.end());

    det.insert(det.end(), add.begin(), add.end());

    sort(det.begin(), det.end());


    for (auto &x:det) {
        tmp_SHA << calculate_sha1(x);
    }
    det.clear();
    //防止del的被算文件哈希(无法读文件），改为算文件路径哈希
    det.insert(det.end(), info.del.begin(), info.del.end());
    sort(det.begin(), det.end());
    for (auto &x:det) {
        tmp_SHA << calculate_string_sha1(x);
    }


    new_sha1 = calculate_string_sha1(tmp_SHA.str());


    char tmp_time[500];
    auto tmpp = database::getCurrentTimeChar();

    strcpy(tmp_time, tmpp);
    free(tmpp);

    //创建新节点
    sprintf(sql,
            "INSERT INTO Node (SHA,CreatedDateTime,Parent,Message) VALUES ('%s','%s',(SELECT SHA FROM Node WHERE SHA='%s'),'%s' )",
            new_sha1.c_str(), tmp_time, branch_head.c_str(), Message);

    rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]新节点创建失败：" << zErrMsg << endl;
        exit(1);
    } else {
        clog << "[INFO]新节点创建成功！" << endl;
    }

    //连接新节点和分支
    sprintf(sql,
            "INSERT INTO Node2Branch (ID,Node,Branch,CreatedDateTime) VALUES (NULL,(SELECT SHA FROM Node WHERE SHA='%s'),(SELECT ID FROM Branch WHERE ID='%d'),'%s')",
            new_sha1.c_str(), current_branch, tmp_time);
    rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]新节点与分支连接失败：" << zErrMsg << endl;
        exit(1);
    } else {
        clog << "[INFO]新节点与分支连接成功！" << endl;
    }

    //将当前分支头节点改为新节点
    sprintf(sql, "UPDATE Branch SET BranchHead=(SELECT SHA FROM Node WHERE SHA='%s') WHERE ID='%d'", new_sha1.c_str(),
            current_branch);
    rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]当前分支头节点更改失败：" << zErrMsg << endl;
        exit(1);
    } else {
        clog << "[INFO]当前分支头节点更改成功！" << endl;
    }

    tmpp = database::getCurrentTimeChar();
    strcpy(tmp_time, tmpp);
    free(tmpp);

    //将add信息加入到连接表和Objects表中
    for (auto &p:add) {

        struct stat buf;
        stat(p.c_str(), &buf);

        Compress compress_tmp;
        compress_return compress_info = compress_tmp.compress(p);

        //clog << "compress sha  " << compress_info.sha1 << endl;
        //clog << "compress path  " << compress_info.compressed_path << endl;

        sprintf(sql,
                "INSERT INTO Objects (CompressedSHA,CompressedPath,OriginSHA,OriginPath,CreatedDateTime,UpdatedDateTime) VALUES ('%s','%s','%s','%s','%s','%s')",
                compress_info.sha1.c_str(), compress_info.compressed_path.c_str(), calculate_sha1(p), p.c_str(),
                database::getTimeChar(buf.st_ctime), database::getTimeChar(buf.st_mtime));
        rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }


        sprintf(sql,
                "INSERT INTO Obj2Node (ID,File,Mode,Node,CreatedDateTime) VALUES (NULL,(SELECT CompressedSHA FROM Objects WHERE CompressedSHA='%s'),1,(SELECT SHA FROM Node WHERE SHA='%s'),'%s')",
                compress_info.sha1.c_str(), new_sha1.c_str(), tmp_time);
        rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }

    //把del信息加入连接表中
    for (auto &p:info.del) {
        clog << "compress sha  " << info.path2SHA[p] << endl;

        sprintf(sql,
                "INSERT INTO Obj2Node (ID,File,Mode,Node,CreatedDateTime) VALUES (NULL,(SELECT CompressedSHA FROM Objects WHERE CompressedSHA='%s' ),-1,(SELECT SHA FROM Node WHERE SHA='%s'),'%s')",
                info.path2SHA[p].c_str(), new_sha1.c_str(), tmp_time);
        rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }

    //把change信息加入连接表和Objects表中
    for (auto &p:info.change) {
        struct stat buf;
        stat(p.c_str(), &buf);

        Compress compress_tmp;
        compress_return compress_info = compress_tmp.compress(p);

        sprintf(sql,
                "INSERT INTO Objects (CompressedSHA,CompressedPath,OriginSHA,OriginPath,CreatedDateTime,UpdatedDateTime) VALUES ('%s','%s','%s','%s','%s','%s')",
                compress_info.sha1.c_str(), compress_info.compressed_path.c_str(), calculate_sha1(p), p.c_str(),
                database::getTimeChar(buf.st_ctime), database::getTimeChar(buf.st_mtime));
        rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }

        sprintf(sql,
                "INSERT INTO Obj2Node (ID,File,Mode,Node,CreatedDateTime) VALUES (NULL,(SELECT CompressedSHA FROM Objects WHERE CompressedSHA='%s' ),0,(SELECT SHA FROM Node WHERE SHA='%s'),'%s')",
                info.path2SHA[p].c_str(), new_sha1.c_str(), tmp_time);
        rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
    }

    clog << "[INFO]commit完成！" << endl;
    sqlite3_close(db);
}
