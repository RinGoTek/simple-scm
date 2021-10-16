//
// Created by login on 2021/6/23.
//

#include "test_checkout.h"
#include"../module_checkout.h"
#include<iostream>
#include "ctime"
using namespace std;

void test_checkout::test_switch_node() {
    module_checkout tmp;
    cout<<"请输入节点哈希：";
    string s;
    cin>>s;
    clock_t start_time = clock();
    tmp.checkout_switch_node(const_cast<char *>(s.c_str()),true);

    cout << "checkout完成！耗时：" << 1.0 * (clock() - start_time) / 1000.0 << "ms" << endl;
}

void test_checkout::test_switch_branch() {
    module_checkout tmp;
    cout<<"请输入分支名：";
    string s;
    cin>>s;
    clock_t start_time = clock();
    tmp.checkout_switch_branch(const_cast<char *>(s.c_str()));

    cout << "checkout完成！耗时：" << 1.0 * (clock() - start_time) / 1000.0 << "ms" << endl;
}
