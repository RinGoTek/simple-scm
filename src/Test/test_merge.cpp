//
// Created by login on 2021/6/23.
//

#include "test_merge.h"
#include<iostream>
#include "ctime"
#include"../module_merge.h"

using namespace std;

void test_merge::test() {
    cout<<"请输入要合并进来的节点sha：";
    string s;
    cin>>s;
    module_merge tmp;
    clock_t start_time = clock();
    tmp.merge(const_cast<char *>(s.c_str()));

    cout << "merge完成！耗时：" << 1.0 * (clock() - start_time) / 1000.0 << "ms" << endl;
}
