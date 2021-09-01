//
// Created by a2447742618 on 2021/5/28.
//

#ifndef SIMPLE_SCM_MODULE_COMMIT_H
#define SIMPLE_SCM_MODULE_COMMIT_H

#include"base_module.h"
class module_commit :public virtual base_module{
public:
    module_commit()
    {
        check_repository_version();
    }
    void commit(char *);
    void help() override;
};


#endif //SIMPLE_SCM_MODULE_COMMIT_H
