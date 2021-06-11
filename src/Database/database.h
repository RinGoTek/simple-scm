//
// Created by login on 2021/5/25.
// 这是数据库的类的头文件
//

#ifndef SIMPLE_SCM_DATABASE_H
#define SIMPLE_SCM_DATABASE_H


#include <string>
#include<time.h>
#include"init-db.h"
#include<string>

struct node_full_info
{
    std::string SHA;
    std::string CreatedDateTime;
    std::string Parent;
    std::string Message;
};

class database {

    friend class init_db;

public:
    //database类的默认构造函数
    database() = default;

    //初始化存储仓库的函数（调用的是init-db类）
    void init();

    //获取当前时间的函数
    static tm *getCurrentTime();

    //获取当前已格式化的，可存入数据库的时间字符串的函数
    static char *getCurrentTimeChar();

    //将time_h转化为char *类型的函数
    static char *getTimeChar(time_t);

private:


};


#endif //SIMPLE_SCM_DATABASE_H
