//
// Created by login on 2021/5/16.
//

#ifndef SIMPLE_SCM_CACHE_H
#define SIMPLE_SCM_CACHE_H

#endif //SIMPLE_SCM_CACHE_H

#include<iostream>

//stat.h用来获取文件的属性这些都很方便
#include<sys/stat.h>

//fcntl.h，是unix标准中通用的头文件，其中包含的相关函数有 open，fcntl，shutdown，unlink，fclose等
#include<fcntl.h>

#include<cstdlib>
#include<cstdarg>
//errno.h 是C语言C标准函式库里的标头档，定义了通过错误码来回报错误资讯的宏。
#include<cerrno>


#include<openssl/sha.h>
#include<zlib.h>
#include <unistd.h>

/*
 * 这是文件夹缓存的基础数据结构
 */


const std::string DB_ENVIRONMENT = "SHA1_FILE_DIRECTORY";
const std::string DEFAULT_DB_ENVIRONMENT = ".simple-scm/objects";


//获取当前工作目录
static std::string cwd = get_current_dir_name();
