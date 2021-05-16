//
// Created by login on 2021/5/16.
//

#include<memory>
#include"headers/cache.h"
#include "headers/init-db.h"
#include<string>
#include<stdlib.h>
#include<cstring>

using namespace std;

char cc[16] = {'0', '1', '2', '3', '4', '5','6','7','8','9','a','b','c','d','e','f'};

init_db::init_db()
{
};

//初始化数据仓库的函数
void init_db::do_init() {

    clog << "正在初始化存储库" << endl;

    if (mkdir(".simple-scm", 0700) < 0) {
        if(errno == EEXIST)
        {
            clog<<"存储库已经存在"<<endl;
            exit(1);
        }
        else
        {
            cerr << "创建存储库失败" << endl;
            exit(1);
        }

    }

    //创建根目录
    clog<<"正在创建数据根目录..."<<endl;


    string sha1_dir = DEFAULT_DB_ENVIRONMENT;
    clog<<"正在创建数据目录..."<<endl;

    if(mkdir(sha1_dir.c_str(), 0700)<0)
    {
        if(errno != EEXIST)
        {
            cout<<"发生错误"<<endl;
            exit(1);
        }
    }

    for(char i : cc)
    {
        for(char j : cc)
        {
            if(mkdir((sha1_dir + "/" + i + j).c_str(), 0700) < 0)
            {
                if(errno != EEXIST)
                {
                    cerr<<"发生错误： "<<sha1_dir + "/" + i + j<<endl;
                    exit(1);
                }
            }
        }
    }

    clog<<"创建数据目录成功！"<<endl;



}
