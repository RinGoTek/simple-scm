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
#include <sys/stat.h>
#include<openssl/sha.h>
#include<fstream>

using namespace std;

//将要被忽略的文件、路径
static vector<string> ignore;

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
vector<string> walk_folder(string base_dir) {
    vector<string> ans;

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
    char *sql = "SELECT Path From IgnoreList";
    rc = sqlite3_exec(db, sql, select_ignore_callback, nullptr, &zErrMsg);

    if (rc != SQLITE_OK) {
        cerr << "[ERROR]发生错误：" << zErrMsg << endl;
        exit(1);
    }

    sqlite3_close(db);

    //要遍历的目录
    stack<string> walk_list;
    walk_list.push(base_dir);

    while (!walk_list.empty()) {
        string current_dir = walk_list.top();
        walk_list.pop();

        walk_return wk = do_walk_folder(current_dir);

        //判断文件夹是否在忽略列表中
        for (const auto &x:wk.dirs) {
            auto it = find(ignore.begin(), ignore.end(), x);

            //不在忽略列表中
            if (it == ignore.end())
                walk_list.push(x);
        }

        //判断文件夹是否在忽略列表中
        for (const auto &x:wk.files) {
            auto it = find(ignore.begin(), ignore.end(), x);

            //不在忽略列表中
            if (it == ignore.end())
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
        cout << base_dir << endl;
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

/**
 * 判断是否是文件,
 * */
bool is_file(std::string path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
};

/**
 * 判断是否是一个文件夹,
 * */
bool is_dir(std::string path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
};

/**
 * 计算字符串的sha1
 * @param str 待计算的字符串
 * @return sha1
 */
char *calculate_string_sha1(const string &str) {
    return calculate_char_sha1(str.c_str());
}

/**
 * 计算char×字符串的sha1
 * @param str 待计算的字符串
 * @return sha1
 */
char *calculate_char_sha1(const char *str) {
    unsigned char opt[21];
    char *res = new char[41];
    unsigned long long size = strlen(str) * sizeof(unsigned char);
    SHA1((unsigned char *) str, size, opt);

    int j = 0;
    for (int i = 0; i < 20; i++) {
        char tmp[5];
        //把十六进制转成字符串
        sprintf(tmp, "%02x", opt[i]);
        res[j] = tmp[0];
        res[j + 1] = tmp[1];
        j += 2;
    }
    res[40] = '\0';
    //cout << res << endl;

    return res;


}

char *calculate_sha1(const string &path) {
    /**  计算文件sha1的函数
     *  返回char×
     ×  文件无法访问时返回nullptr
     */
    //以二进制读的方式打开文件
    ifstream fin(path, ios::in | ios::binary);
    if (!fin) {
        return nullptr;
    }

    filebuf *pbuf;
    //获取fin对应的buffer对象的指针
    pbuf = fin.rdbuf();

    //调用buffer对象方法获取文件大小
    long long size = pbuf->pubseekoff(0, ios::end, ios::in);

    pbuf->pubseekpos(0, ios::in);

    //分配内存空间
    char *buffer = new char[size + 1];

    //获取文件内容
    pbuf->sgetn(buffer, size);
    buffer[size] = '\0'; //这是关键
    fin.close();

    unsigned char opt[21];
    char *res = new char[41];

    SHA1((unsigned char *) buffer, size, opt);

    int j = 0;
    for (int i = 0; i < 20; i++) {
        char tmp[5];
        //把十六进制转成字符串
        sprintf(tmp, "%02x", opt[i]);
        res[j] = tmp[0];
        res[j + 1] = tmp[1];
        j += 2;
    }
    //cout << res << endl;
    res[41] = '\0';
    //释放内存
    delete[] buffer;

    return res;
}

void CopyFile(const char* src, const char* dst)//将路径sourcefile的文件在路径new_file处复制一份
{
string command = "cp ";
command+= src;
command+= " ";
command+= dst;
system((char*)command.c_str());
}

int FileRemove(const char* filename)//const char*,删除文件
{
    return remove(filename);
}//删除成功返回值为0，删除失败返回值为-1