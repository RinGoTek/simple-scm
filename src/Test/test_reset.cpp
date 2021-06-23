//
// Created by login on 2021/6/23.
//

#include "test_reset.h"
#include<iostream>
#include "ctime"
#include"../module_reset.h"
using namespace std;
void test_reset::test() {
    cout<<"请输入节点sha：";
    string s;
    cin>>s;
    module_reset tmp;
    clock_t start_time = clock();
    tmp.reset(const_cast<char *>(s.c_str()));

    cout << "reset完成！耗时：" << 1.0 * (clock() - start_time) / 1000.0 << "ms" << endl;


}
