//
// Created by login on 2021/6/5.
//

#include "Compress.h"
#include<iostream>
#include<fstream>
#include"../libs/bundle/bundle.h"
#include"file_system.h"

using namespace std;

/**
     * 压缩单个文件
     * @param path 文件路径
     * @return compress_return
     */
compress_return Compress::compress(const std::string &path) {

    string fileName;

    //不是文件就返回空的compress_return
    if (!is_file(path))
        return compress_return();

        /*
    //分离文件名
    for (unsigned long long i = path.length() - 1; i >= 0; --i) {
        if (path[i] == '/' || path[i] == '\\') {
            fileName = path.substr(i + 1, path.length());
            break;
        }
        if (i == 0) {
            fileName = path;
        }
    }
*/
    //以二进制方式读入文件
    ifstream ifs(path, ios::binary);
    stringstream ss;
    ss << ifs.rdbuf();
    string original = ss.str();
    ifs.close();
    //采用LZ4F算法进行压缩
    string packed = bundle::pack(BUNDLE_LZ4F, original);

    compress_return ret;
    //sha1计算的是读入文件以后的string的sha1
    ret.sha1 = calculate_string_sha1(packed);

    ret.compressed_path = ".simple-scm/objects/" + ret.sha1;
    ofstream fout(ret.compressed_path, ios::binary);
    fout << packed;
    fout.close();

    return ret;
}

/**
     * 压缩单个文件并保存到objects
     * @param path 文件路径
     * @return compress_return
     */
std::vector<compress_return> Compress::batch_compress(const std::vector<std::string> &path) {
    vector<compress_return> ret;
    //提前分配空间，提升性能
    ret.reserve(path.size());
    //逐一进行压缩
    for (const auto &x:path)
        ret.emplace_back(compress(x));
    return ret;
}




