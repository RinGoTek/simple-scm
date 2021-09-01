//
// Created by login on 2021/6/10.
//

#ifndef SIMPLE_SCM_MODULE_MERGE_H
#define SIMPLE_SCM_MODULE_MERGE_H

#include<string>
#include<sqlite3.h>
#include <iostream>
#include<vector>
#include <fstream>
#include"module_detect_changes.h"
#include "headers/global.h"
#include "base_module.h"

class module_merge : public virtual base_module {
public:
    module_merge() {

        check_repository_version();

        this->rc = sqlite3_open(".simple-scm/simple-scm.db", &this->db);
        if (this->rc) {
            std::cerr << "[ERROR]数据库加载失败！" << std::endl;
            exit(1);
        } else {
            if (DEV_MODE)
                std::clog << "[INFO]数据库加载成功！" << std::endl;
        }

        std::ifstream fin(".simple-scm/current_branch.txt");
        std::string tmp;
        fin >> tmp;
        fin.close();
        current_branch = std::atoi(tmp.c_str());
    }

    ~module_merge() = default;

    /**
     * 执行合并
     * @param node2 要被合并进来的节点的sha1
     * @param currentBranch 当前分支的编号
     */
    void merge(const std::string &node2);

    void help() override;

private:
    sqlite3 *db{};
    char *zErrMsg = 0;
    int rc;
    int current_branch;


};


#endif //SIMPLE_SCM_MODULE_MERGE_H
