//
// Created by login on 2021/5/16.
//


#ifdef _WIN32
#include <io.h>
#include <tchar.h>
#include <direct.h>
#include <Windows.h>
#else
#include <dirent.h>
//stat.h用来获取文件的属性这些都很方便
#include<sys/stat.h>
#include <sys/types.h>
//fcntl.h，是unix标准中通用的头文件，其中包含的相关函数有 open，fcntl，shutdown，unlink，fclose等
#include<fcntl.h>
#endif

#ifndef SIMPLE_SCM_GLOBAL_H
#define SIMPLE_SCM_GLOBAL_H
#include<string>
#include<iostream>





#include<cstdlib>
#include<cstdarg>
//errno.h 是C语言C标准函式库里的标头档，定义了通过错误码来回报错误资讯的宏。
#include<cerrno>


#include<openssl/sha.h>

#include <unistd.h>




/*
 * 这是文件夹缓存的基础数据结构
 */


const std::string DEFAULT_DB_ENVIRONMENT = ".simple-scm/objects";


//获取当前工作目录
static std::string cwd = get_current_dir_name();


//是否是开发模式
static bool DEV_MODE = false;

//当前版本名和版本号
static std::string SIMPLE_SCM_VERSION = "0.1.2";
static int SIMPLE_SCM_VERSION_NUMBER = 3;



// 获取当前进程的路径(也就是Simple-SCM程序所在的路径)
static std::string GET_SIMPLE_SCM_PATH() {
    char current_absolute_path[1024];
    int cnt = readlink("/proc/self/exe", current_absolute_path, 1024);
    if (cnt < 0 || cnt >= 1024)
    {
        printf("***Error***\n");
        exit(-1);
    }
//获取当前目录绝对路径，即去掉程序名
    int i;
    for (i = cnt; i >=0; --i)
    {
        if (current_absolute_path[i] == '/')
        {
            current_absolute_path[i+1] = '\0';
            break;
        }
    }
    return current_absolute_path;
}




#endif //SIMPLE_SCM_GLOBAL_H
