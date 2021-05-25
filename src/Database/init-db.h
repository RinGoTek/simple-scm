//
// Created by login on 2021/5/16.
//

#ifndef SIMPLE_SCM_INIT_DB_H
#define SIMPLE_SCM_INIT_DB_H

#include"database.h"



class init_db {
private:
    init_db() = default;

    //初始化数据仓库的函数
    void do_init();

    //初始化数据库和数据表的函数
    void database_init();

    //声明database类为init-db的友元类
    friend class database;

};
#endif //SIMPLE_SCM_INIT_DB_H