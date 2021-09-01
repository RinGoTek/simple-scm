//
// Created by a2447742618 on 2021/5/29.
//

#ifndef SIMPLE_SCM_MODULE_NEW_BRANCH_H
#define SIMPLE_SCM_MODULE_NEW_BRANCH_H

#include<string>
#include"headers/global.h"
#include"base_module.h"

using namespace std;


class module_new_branch : public virtual base_module {
public:
    module_new_branch()
    {
        check_repository_version();
    }
    void create_branch(char[]);

    void help() override;
};


#endif //SIMPLE_SCM_MODULE_NEW_BRANCH_H
