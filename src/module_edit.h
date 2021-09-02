//
// Created by login on 2021/8/31.
//
//这是用来编辑各项已经存在的内容的类

#ifndef SIMPLE_SCM_MODULE_EDIT_H
#define SIMPLE_SCM_MODULE_EDIT_H

#include "base_module.h"
#include<map>

class module_edit : public virtual base_module {
public:
    module_edit() {
        check_repository_version();
    }

    void edit(char *parameters[], int size);



    void help() override;

private:
    std::map<std::string, std::string> mp;
    std::vector<std::pair<std::string, std::string> > ops;

    void edit_node_text();
};


#endif //SIMPLE_SCM_MODULE_EDIT_H
