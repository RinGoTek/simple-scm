//
// Created by login on 2021/8/31.
//
//这是一个虚基类，所有的module都要继承自它
#ifndef SIMPLE_SCM_BASE_MODULE_H
#define SIMPLE_SCM_BASE_MODULE_H

#include<fstream>
#include<iostream>
#include<string>
#include <sstream>
#include "headers//global.h"
#include<vector>

class base_module {
public:
    base_module()=default;

    //检验存储库版本
    void check_repository_version();

    ~base_module() = default;

    //每个继承的子类都必须重写这个help方法
    virtual void help() = 0;

    //解析操作
    std::vector<std::pair<std::string , std::string > >parse_ops(char *parameters[], int size);

private:

};


#endif //SIMPLE_SCM_BASE_MODULE_H
