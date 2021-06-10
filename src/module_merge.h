//
// Created by login on 2021/6/10.
//

#ifndef SIMPLE_SCM_MODULE_MERGE_H
#define SIMPLE_SCM_MODULE_MERGE_H
#include<string>

class module_merge {
public:
    module_merge() = default;
    ~module_merge() = default;
    /**
     * 执行合并
     * @param node1 第一个节点的sha1
     * @param node2 第二个节点的sha1
     * @param currentBranch 当前分支的编号
     */
    void merge(const std::string& node1, const std::string& node2, const int &currentBranch);
};


#endif //SIMPLE_SCM_MODULE_MERGE_H
