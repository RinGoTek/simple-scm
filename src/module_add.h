//
// Created by a2447742618 on 2021/6/1.
//

#ifndef SIMPLE_SCM_MODULE_ADD_H
#define SIMPLE_SCM_MODULE_ADD_H

#include"base_module.h"
class module_add :public virtual base_module{
public:
    void add(char *);
    void help() override;
};


#endif //SIMPLE_SCM_MODULE_ADD_H
