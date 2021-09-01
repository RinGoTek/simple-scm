//
// Created by login on 2021/5/25.
// 这是数据库的类
//

#include "database.h"

#include"init-db.h"
#include<iostream>
#include<sstream>
#include<iomanip>
#include<cstring>

using namespace std;

//初始化存储仓库
void database::init() {
    init_db idb;
    idb.do_init();
}

//获取当前系统时间
tm *database::getCurrentTime() {


    time_t tt;
    time(&tt);
    tt = tt + 8 * 3600;  // transform the time zone
    tm *t = gmtime(&tt);

    return t;
}

char* format_string(tm* t)
{
    stringstream fmt;

    fmt << t->tm_year + 1900 << '-' << setw(2) << setfill('0') << t->tm_mon + 1 << '-' << setw(2) << setfill('0')
        << t->tm_mday
        << ' ' << setw(2) << setfill('0') << t->tm_hour << ':' << setw(2) << setfill('0') << t->tm_min << ':' << setw(2)
        << setfill('0') << t->tm_sec;
    string str = fmt.str();

    char *res = new char[str.size() + 1];
    strcpy(res, str.c_str());
    return res;
}


char *database::getCurrentTimeChar() {
    tm *tm_local = getCurrentTime();
    auto t = tm_local;
    return format_string(t);

}

char *database::getTimeChar(time_t tt) {

    tt = tt + 8 * 3600;  // transform the time zone
    tm *t = gmtime(&tt);
    return format_string(t);

}

void database::exec(sqlite3 *db,  char* sql, char *zErrMsg, int* callback) {
    int rc;
    rc = sqlite3_exec(db, sql, reinterpret_cast<int (*)(void *, int, char **, char **)>(callback), NULL, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }
}

void database::exec_with_transaction(sqlite3 *db, char *sql, char *zErrMsg, int *callback) {
    int rc;
    rc = sqlite3_exec(db, sql, reinterpret_cast<int (*)(void *, int, char **, char **)>(callback), NULL, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;

        //事务回滚
        rc = sqlite3_exec(db, "ROLLBACK;", 0, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "[ERROR]发生错误：" << zErrMsg << endl;
            exit(1);
        }
        exit(1);

    }
}
