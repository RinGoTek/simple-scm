//
// Created by login on 2021/6/1.
//

#ifndef SIMPLE_SCM_FILE_SYSTEM_H
#define SIMPLE_SCM_FILE_SYSTEM_H

#endif //SIMPLE_SCM_FILE_SYSTEM_H
#include<string>
#include<vector>
#include<sys/stat.h>
using namespace std;
#include "../headers/cache.h"

/**
 * 遍历目录下的所有文件
 * @param: base_dir 基础目录，默认是cwd
 * @return
 */
std::vector<std::string> walk_folder(std::string base_dir);

/**
 *将路径src的文件在路径dst处复制一份
 * @param src源文件路径
 * @param dst 目标路径
 */
void CopyFile(const char* src, const char* dst);//将路径sourcefile的文件在路径new_file处复制一份

/**
 * 删除路径为filename的文件
 * @param filename 源文件路径
 * @return 删除成功返回值为0,否则为-1
 */
int FileRemove(const char* filename);//const char*,删除文件

/**
 * 判断是否是文件,
 * */
bool is_file(std::string path);


/**
 * 判断是否是一个文件夹,
 * */
bool is_dir(std::string path) ;


/**  计算sha1的函数
     *  返回char×
     ×  文件无法访问时返回nullptr
     */
char* calculate_sha1(const std::string& path);

/**
 * 计算char×字符串的sha1
 * @param str 待计算的字符串
 * @return sha1
 */
char *calculate_char_sha1(const char* str);

/**
 * 计算字符串的sha1
 * @param str 待计算的字符串
 * @return sha1
 */
char *calculate_string_sha1(const string& str);


