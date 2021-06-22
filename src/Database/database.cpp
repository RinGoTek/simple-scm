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
    /*
    stringstream fmt;

    fmt << t->tm_year + 1900 << '-' << setw(2) << setfill('0') << t->tm_mon + 1 << '-' << setw(2) << setfill('0')
        << t->tm_mday
        << ' ' << setw(2) << setfill('0') << t->tm_hour << ':' << setw(2) << setfill('0') << t->tm_min << ':' << setw(2)
        << setfill('0') << t->tm_sec;
    string str = fmt.str();

    char *res = new char[str.size() + 1];
    strcpy(res, str.c_str());
    return res;
    */
}
