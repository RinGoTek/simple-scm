//
// Created by login on 2021/6/1.
//这是用来忽略更改的类
//

#ifndef SIMPLE_SCM_MODULE_IGNORE_H
#define SIMPLE_SCM_MODULE_IGNORE_H

#include<string>
#include"base_module.h"

class module_ignore : public virtual base_module {
public:
    module_ignore()
    {
        check_repository_version();
    }

    //把路径加入到ignore列表
    void do_ignore(std::string path);

    //取消ignore
    void de_ignore(std::string path);

    void help() override;

};


#endif //SIMPLE_SCM_MODULE_IGNORE_H
