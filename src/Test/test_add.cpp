//
// Created by a2447742618 on 2021/6/23.
//

#include "test_add.h"
#include"../module_add.h"
#include<time.h>
#include<iostream>
using namespace std;

void test_add::test_module_add() {
    string path;
    cout<<"请输入添加的路径"<<endl;
    cin>>path;

    clock_t start_time = clock();
    module_add tmp;
    tmp.add(path.data());
    cout << "add完成！耗时：" << 1.0 * (clock() - start_time) / 1000.0 << "ms" << endl;
}
