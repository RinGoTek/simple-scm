//
// Created by login on 2021/6/10.
//

#ifndef SIMPLE_SCM_MODULE_MERGE_H
#define SIMPLE_SCM_MODULE_MERGE_H

#include<string>
#include<sqlite3.h>
#include <iostream>
#include<vector>

class module_merge {
public:
    module_merge() {
        this->rc = sqlite3_open(".simple-scm/simple-scm.db", &this->db);

        if (this->rc) {
            std::cerr << "[ERROR]数据库加载失败！" << std::endl;
            exit(1);
        } else {
            std::clog << "[INFO]数据库加载成功！" << std::endl;
        }
    }

    ~module_merge() = default;

    /**
     * 执行合并
     * @param node1 本分支的节点的sha1
     * @param node2 要被合并进来的节点的sha1
     * @param currentBranch 当前分支的编号
     */
    void merge(const std::string &node1, const std::string &node2, const int &currentBranch);

private:
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;


};


#endif //SIMPLE_SCM_MODULE_MERGE_H
