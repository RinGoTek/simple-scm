//
// Created by login on 2021/6/1.
//

#include "file_system.h"
#include<dirent.h>
#include<iostream>
#include<cstdlib>
#include <cstring>
#include"../headers/cache.h"
#include<sqlite3.h>
#include<algorithm>
#include<stack>

using namespace std;

//将要被忽略的文件、路径
vector<string> ignore;


struct walk_return {
    std::vector<std::string> files;
    std::vector<std::string> dirs;
};

walk_return do_walk_folder(const string &base_dir);

static int select_ignore_callback(void *NotUsed, int cnt, char **pValue, char **pName)//获取ignore信息的函数
{
    //把信息添加到ignore数组
    ignore.emplace_back(pValue[0]);
    return 0;
}

//对外开放的，获取文件夹下所有文件的函数
//返回一个数组，包含所有的文件的（除了ignore以外的）路径信息
vector<string> walk_folder() {
    vector<string> ans;
    string base_dir = cwd;
    ignore.emplace_back(".simple-scm");

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);

    if (rc) {
        cerr << "[ERROR]数据库加载失败！" << endl;
        exit(1);
    } else {
        clog << "[INFO]数据库加载成功！" << endl;
    }

    //获取ignore列表
    char* sql = "SELECT Path From IgnoreList";
    rc = sqlite3_exec(db, sql, select_ignore_callback, nullptr, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    sqlite3_close(db);

    //要遍历的目录
    stack<string> walk_list;
    walk_list.push(".");

    while (!walk_list.empty()) {
        string current_dir = walk_list.top();
        walk_list.pop();

        walk_return wk = do_walk_folder(current_dir);

        //判断文件夹是否在忽略列表中
        for(const auto& x:wk.dirs)
        {
            auto it = find(ignore.begin(), ignore.end(), x);

            //不在忽略列表中
            if(it == ignore.end())
                walk_list.push(x);
        }

        //判断文件夹是否在忽略列表中
        for(const auto& x:wk.files)
        {
            auto it = find(ignore.begin(), ignore.end(), x);

            //不在忽略列表中
            if(it == ignore.end())
               ans.emplace_back(x);
        }
    }


    return ans;
}


//获取当前文件夹下所有文件、文件夹的函数（只获取当前级别的）
walk_return do_walk_folder(const string &base_dir) {
    vector<string> files;
    vector<string> dirs;

#ifdef linux
//在linux下walk_folder
    DIR *dir;

    struct dirent *ptr;

//打开文件夹失败
    if ((dir = opendir(base_dir.c_str())) == NULL) {
        cerr << "[ERROR]打开文件夹失败" << endl;
        exit(1);
    }

    while ((ptr = readdir(dir)) != NULL) {
//是当前路径或者父目录
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if (ptr->d_type == 8)//文件
            files.emplace_back(base_dir + "/" + ptr->d_name);
        else if (ptr->d_type == 10)//link file
            continue;
        else if (ptr->d_type == 4)//文件夹
        {
            dirs.emplace_back(base_dir + "/" + ptr->d_name);
        }
    }

    closedir(dir);


#endif

    walk_return wk;
    wk.dirs = dirs;
    wk.files = files;
    return wk;
}