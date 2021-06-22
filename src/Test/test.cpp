//
// Created by login on 2021/6/22.
//

#include "test.h"
#include<iostream>
#include"../headers/global.h"
#include"test_compress.h"
using namespace std;

//测试入口
void test::start_test() {
    if(!DEV_MODE)
    {
        cout<<"当前不在开发模式下，无法进行测试"<<endl;
        exit(0);
    }
    cout<<"请输入要测试的操作：\n"
        <<"1、压缩单个文件\n";

    int sw;
    cin>>sw;

    switch (sw) {
        case 1:
            test_compress tmp;
            tmp.test_compress_one_file();
            break;
        default:
            cout<<"请输入正确的序号"<<endl;
            exit(0);
            break;

    }

}
