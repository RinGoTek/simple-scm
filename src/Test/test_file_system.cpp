//
// Created by login on 2021/6/22.
//

#include "test_file_system.h"
#include"../Database/file_system.h"
#include<iostream>
#include <ctime>
using namespace std;

void test_file_system::test_walk_folder() {
    //测试walk_folder
    cout<<"请输入路径： ";
    string path;
    cin>>path;

    auto start_time = clock();
    auto ans = walk_folder(path);

    auto end_time = clock();

    for(const auto&x:ans)
        cout<<x<<endl;

    cout<<"共有"<<ans.size()<<"个文件"<<endl;
    cout<<"耗时："<<1.0*(end_time-start_time)/1000.0<<"ms"<<endl;

}
