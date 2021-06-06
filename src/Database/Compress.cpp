//
// Created by login on 2021/6/5.
//

#include "Compress.h"
#include<iostream>
#include<fstream>
#include"../libs/bundle/bundle.h"
#include"file_system.h"
#include<stdexcept>

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

decompress_return Compress::decompress(const string &compressed_path, const string &path) {

    //不是文件就抛出异常
    if (!is_file(compressed_path))
        throw string("The size of vector<compressed_path> must be equal with that of vector<path>");
    //以二进制方式读入文件
    ifstream fin(compressed_path, ios::binary);
    stringstream ss;
    ss << fin.rdbuf();
    string compressed = ss.str();
    fin.close();
    //解压缩
    string unpacked = bundle::unpack(compressed);

    string dirs = get_file_parent_dir(path);

    //先创建文件夹（若已存在会自动忽略）
    custom_mkdirs(dirs);

    ofstream fout(path, ios::binary);
    fout << unpacked << endl;
    fout.close();

    decompress_return ret;
    ret.sha1 = calculate_string_sha1(unpacked);
    ret.decompressed_path = path;
    return ret;
}

/**
     * 批量解压文件到指定路径
     * @param compressed_path 压缩文件的路径
     * @param path 解压到哪里
     * @return vector<decompress_return>
     */
std::vector<decompress_return>
Compress::batch_decompress(const vector<std::string> &compressed_path, const vector<std::string> &path) {
    vector<decompress_return> ret;
    //两者的大小必须相同
    if (compressed_path.size() != path.size())
        throw string("The size of vector<compressed_path> must be equal with that of vector<path>");

    //提前分配空间，提升性能
    auto size = compressed_path.size();
    ret.resize(size);

    //顺序访问，提升性能
    auto compressed_path_iter = compressed_path.begin();
    auto path_iter = path.begin();

    //批量解压
    while (compressed_path_iter != compressed_path.end()) {
        ret.emplace_back(decompress(*compressed_path_iter, *path_iter));
        compressed_path_iter++;
        path_iter++;
    }
    return ret;

}




