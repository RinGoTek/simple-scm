//
// Created by login on 2021/5/16.
//这是初始化存储仓库的类，应当被database类调用
//

#include<memory>
#include"../headers/cache.h"
#include "init-db.h"
#include<string>
#include<stdlib.h>
#include<cstring>
#include<cstdio>
#include<sqlite3.h>
#include<fstream>

#include "database.h"

using namespace std;

static int callback(void *NotUsed, int cnt, char **pValue, char **pName) {
    for (int i = 1; i <= cnt; i++) cout << pName << "=" << (pValue[i] ? pValue[i] : "NULL") << endl;
    return 0;
}


//初始化数据仓库和数据库的函数
void init_db::do_init() {

    clog << "[INFO]正在初始化存储库" << endl;

    if (mkdir(".simple-scm", 0700) < 0) {
        if (errno == EEXIST) {
            clog << "[INFO]存储库已经存在" << endl;
            exit(1);
        } else {
            cerr << "[ERROR]创建存储库失败" << endl;
            exit(1);
        }

    }

    //创建根目录
    clog << "[INFO]正在创建数据根目录..." << endl;


    string sha1_dir = DEFAULT_DB_ENVIRONMENT;
    clog << "[INFO]正在创建数据目录..." << endl;

    if (mkdir(sha1_dir.c_str(), 0700) < 0) {
        if (errno != EEXIST) {
            cerr << "[ERROR]发生错误" << endl;
            exit(1);
        }
    }


    if (mkdir((sha1_dir + "/objects").c_str(), 0700) < 0) {
        if (errno != EEXIST) {
            cerr << "[ERROR]发生错误： " << sha1_dir + "/objects" << endl;
            exit(1);
        }
    }


    clog << "[INFO]创建数据目录成功！" << endl;


    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);

    if (rc) {
        cerr << "[ERROR]数据库创建失败！" << endl;
        exit(1);
    } else {
        clog << "[INFO]数据库创建成功！" << endl;
    }

    sql = "CREATE TABLE Objects("\
        "CompressedSHA CHAR(40) PRIMARY KEY,"\
        "OriginSHA CHAR(40) NOT NULL,"\
        "OriginPath TEXT(500) NOT NULL,"\
        "CompressedPath TEXT(500) NOT NULL,"\
        "CreatedDateTime DATETIME NOT NULL,"\
        "UpdatedDateTime DATETIME NOT NULL);";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据表Object创建失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]数据表Object创建成功！" << endl;
    }

    sql = "CREATE TABLE Node("\
        "SHA CHAR(40) PRIMARY KEY,"\
        "CreatedDateTime DATETIME NOT NULL,"\
        "Parent CHAR(40),"\
        "Message TEXT(1000) NOT NULL,"\
        "FOREIGN KEY (Parent) REFERENCES Node(SHA) ON DELETE CASCADE);";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据表Node创建失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]数据表Node创建成功！" << endl;
    }

    sql = "CREATE TABLE Obj2Node("\
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"\
        "File CHAR(40) NOT NULL,"\
        "NODE CHAR(40) NOT NULL,"\
        "Mode INTEGER NOT NULL,"\
        "CreatedDateTime DATETIME NOT NULL,"\
        "FOREIGN KEY (File) REFERENCES Objects(CompressedSHA) ON DELETE CASCADE,"\
        "FOREIGN KEY (NODE) REFERENCES Node(SHA) ON DELETE CASCADE);";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据表Obj2Node创建失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]数据表Obj2Node创建成功！" << endl;
    }

    sql = "CREATE TABLE Branch("\
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"\
        "Name CHAR(40) NOT NULL,"\
        "BranchRoot CHAR(40) NOT NULL,"\
        "BranchHead CHAR(40) NOT NULL,"\
        "CreatedDateTime DATETIME NOT NULL,"\
        "UpdatedDateTime DATETIME NOT NULL,"\
        "FOREIGN KEY (BranchHead) REFERENCES Node(SHA) ON DELETE CASCADE,"\
        "FOREIGN KEY (BranchRoot) REFERENCES Node(SHA) ON DELETE CASCADE);";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据表Branch创建失败：" << zErrMsg << endl;
    } else {
        clog << "[INFO]数据表Branch创建成功！" << endl;
    }

    sql = "CREATE TABLE Node2Branch("\
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "\
        "Node CHAR(40) NOT NULL,"\
        "Branch INTEGER NOT NULL,"\
        "CreatedDateTime DATETIME NOT NULL,"\
        "FOREIGN KEY (Node) REFERENCES Node(SHA) ON DELETE CASCADE,"\
        "FOREIGN KEY (Branch) REFERENCES Branch(ID) ON DELETE CASCADE);";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据表Node2Branch创建失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]数据表Node2Branch创建成功！" << endl;
    }

    sql = "CREATE TABLE AddList("\
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"\
        "OriginSHA CHAR(40) NOT NULL,"\
        "OriginPath TEXT(500) NOT NULL,"\
        "CreatedDateTime DATETIME NOT NULL,"\
        "UpdatedDateTime DATETIME NOT NULL);";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据表AddList创建失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]数据表AddList创建成功！" << endl;
    }

    //创建根节点
    char tmp_time[100];

    strcpy(tmp_time, database::getCurrentTimeChar());

    char tmp_sql[1000];

    sprintf(tmp_sql, "INSERT INTO Node (SHA, CreatedDateTime, Message) VALUES ('000000', '%s','Initialize');", tmp_time);


    rc = sqlite3_exec(db, tmp_sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]根节点创建失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]根节点创建成功！" << endl;
    }

    //创建主分支
    strcpy(tmp_time, database::getCurrentTimeChar());

    sprintf(tmp_sql, "INSERT INTO Branch (ID,Name, BranchRoot, BranchHead, CreatedDateTime, UpdatedDateTime) VALUES ((NULL),'main', (SELECT SHA FROM Node WHERE SHA = '000000'), (SELECT SHA FROM Node WHERE SHA = '000000'),'%s', '%s')", tmp_time, tmp_time);

    rc = sqlite3_exec(db, tmp_sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]main分支创建失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]main分支创建成功！" << endl;
    }


    //连接主分支和根节点
    strcpy(tmp_time, database::getCurrentTimeChar());

    sprintf(tmp_sql,"INSERT INTO Node2Branch (ID,Node,Branch,CreatedDateTime) VALUES (NULL, (SELECT SHA FROM Node WHERE SHA = '000000'), (SELECT ID FROM Branch WHERE ID = 1),'%s')",tmp_time);

    rc = sqlite3_exec(db, tmp_sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]根节点与主分支连接失败: " << zErrMsg << endl;
    } else {
        clog << "[INFO]根节点与主分支连接成功！" << endl;
    }

    clog << "[INFO]simple-scm存储库初始化完毕！" << endl;
    sqlite3_close(db);

    //头指针默认指向main分支
    ofstream file("current_branch.txt");
    file<<1;
    file.close();
}

