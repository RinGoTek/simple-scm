//
// Created by login on 2021/6/22.
//

#include "test_compress.h"
#include"../Database/Compress.h"
#include"../Database/file_system.h"
#include<iostream>
#include <ctime>

using namespace std;

//测试压缩单个文件
void test_compress::test_compress_one_file() {
    cout << "请输入文件路径: ";
    string path;
    cin >> path;
    Compress tmp;
    clock_t start_time = clock();
    tmp.compress(path);

    cout << "压缩完成！耗时：" << 1.0 * (clock() - start_time) / 1000.0 << "ms" << endl;


}

//测试压缩目录下的所有文件
void test_compress::test_compress_files() {
    cout << "请输入路径： ";
    string path;
    cin >> path;

    auto file_list = walk_folder(path);
    Compress tmp;
    clock_t start_time = clock();
    tmp.batch_compress(file_list);
    cout << "压缩完成！耗时：" << 1.0 * (clock() - start_time) / 1000.0 << "ms" << endl;

}
