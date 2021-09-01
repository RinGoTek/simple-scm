//
// Created by a2447742618 on 2021/5/29.
//

#ifndef SIMPLE_SCM_MODULE_LIST_H
#define SIMPLE_SCM_MODULE_LIST_H

#include "base_module.h"

class module_list : public virtual base_module {
public:
    module_list()
    {
        check_repository_version();
    }
    void list();

    void help() override;

};


#endif //SIMPLE_SCM_MODULE_LIST_H
