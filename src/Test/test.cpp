//
// Created by login on 2021/6/22.
//

#include "test.h"
#include<iostream>
#include"../headers/global.h"
#include"test_compress.h"
#include "test_database.h"
#include"test_file_system.h"
#include"test_commit.h"
#include"test_add.h"
#include "test_checkout.h"

using namespace std;

//测试入口
void test::start_test() {
    if (!DEV_MODE) {
        cout << "当前不在开发模式下，无法进行测试" << endl;
        exit(0);
    }
    cout << "请输入要测试的操作：\n"
         << "1、压缩单个文件\n"
         << "2、压缩目录下的所有文件\n"
         << "3、测试get_current_time_char\n"
         << "4、测试getTimeChar\n"
         << "5、测试walk_folder\n"
         << "6、测试commit\n"
         << "7、测试add\n"
         << "8、测试切换节点\n"
         << "9、测试切换分支\n";

    int sw;
    cout << ">> ";
    cin >> sw;

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
        case 6:
            test_commit tmp_6;
            tmp_6.test_module_commit();
            break;
        case 7:
            test_add tmp_7;
            tmp_7.test_module_add();
            break;
        case 8:
            test_checkout tmp_8;
            tmp_8.test_switch_node();
            break;
        case 9:
            test_checkout tmp_9;
            tmp_9.test_switch_branch();
            break;
        default:
            cout << "请输入正确的序号" << endl;
            exit(0);

    }

}
