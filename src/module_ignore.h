//
// Created by login on 2021/6/1.
//这是用来忽略更改的类
//

#ifndef SIMPLE_SCM_MODULE_IGNORE_H
#define SIMPLE_SCM_MODULE_IGNORE_H
#include<string>

class module_ignore {
public:
    module_ignore()=default;
    //把路径加入到ignore列表
    void do_ignore(std::string path);
    //取消ignore
    void deIgnore(std::string path);

};


#endif //SIMPLE_SCM_MODULE_IGNORE_H
