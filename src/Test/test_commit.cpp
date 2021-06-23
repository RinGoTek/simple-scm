//
// Created by a2447742618 on 2021/6/22.
//

#include "test_commit.h"
#include "../module_commit.h"
#include<iostream>
#include"ctime"
using namespace std;

void test_commit::test_module_commit()
{
    string message;
    cout<<"请输入节点更改信息"<<endl;
    cin>>message;

    clock_t start_time = clock();
    module_commit tmp;
    tmp.commit(message.data());
    cout << "commit完成！耗时：" << 1.0 * (clock() - start_time) / 1000.0 << "ms" << endl;
}
