//
// Created by login on 2021/6/22.
//

#include "test_compress.h"
#include"../Database/Compress.h"
#include<iostream>
#include <ctime>
using namespace std;

void test_compress::test_compress_one_file() {
    cout<<"请输入文件路径: ";
    string path;
    cin>>path;
    Compress tmp;
    clock_t start_time = clock();
    tmp.compress(path);

    cout<<"压缩完成！耗时："<<1.0*(clock()-start_time)/1000.0<<"ms"<<endl;


}

void test_compress::test_compress_files() {

}
