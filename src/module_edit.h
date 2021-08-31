//
// Created by login on 2021/8/31.
//
//这是用来编辑各项已经存在的内容的类

#ifndef SIMPLE_SCM_MODULE_EDIT_H
#define SIMPLE_SCM_MODULE_EDIT_H

#include "base_module.h"

class module_edit : public virtual base_module {
    module_edit() = default;

    void edit(char *parameters[], int size);

    void help() override;
};


#endif //SIMPLE_SCM_MODULE_EDIT_H
