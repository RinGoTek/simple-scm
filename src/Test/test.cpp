//
// Created by login on 2021/6/22.
//

#include "test.h"
#include<iostream>
#include"../headers/global.h"
#include"test_compress.h"
#include "test_database.h"
#include"test_file_system.h"
using namespace std;

//测试入口
void test::start_test() {
    if(!DEV_MODE)
    {
        cout<<"当前不在开发模式下，无法进行测试"<<endl;
        exit(0);
    }
    cout<<"请输入要测试的操作：\n"
        <<"1、压缩单个文件\n"
        <<"2、压缩目录下的所有文件\n"
        <<"3、测试get_current_time_char\n"
        <<"4、测试getTimeChar\n"
        <<"5、测试walk_folder\n";

    int sw;
    cin>>sw;

    switch (sw) {
        case 1:
            test_compress tmp_1;
            tmp_1.test_compress_one_file();
            break;
        case 2:
            test_compress tmp_2;
            tmp_2.test_compress_files();
            break;
        case 3:
            test_database tmp_3;
            tmp_3.test_get_current_time_char();
            break;
        case 4:
            test_database tmp_4;
            tmp_4.test_get_time_char();
            break;
        case 5:
            test_file_system tmp_5;
            tmp_5.test_walk_folder();
            break;
        default:
            cout<<"请输入正确的序号"<<endl;
            exit(0);
            break;

    }

}
