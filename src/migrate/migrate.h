//
// Created by login on 2021/9/1.
//

#ifndef SIMPLE_SCM_MIGRATE_H
#define SIMPLE_SCM_MIGRATE_H

#include "../headers/global.h"
#include "sqlite3.h"
class migrate {
public:
    migrate()=default;
    ~migrate()=default;
    void action();
private:
    void migrate_to_3();
    bool check_version();

    int ver;
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[500];


};


#endif //SIMPLE_SCM_MIGRATE_H
