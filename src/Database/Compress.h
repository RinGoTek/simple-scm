//
// Created by login on 2021/6/5.
//

#ifndef SIMPLE_SCM_COMPRESS_H
#define SIMPLE_SCM_COMPRESS_H
#include<string>
#include<vector>


struct compress_return
{

    std::string sha1;   //压缩后的文件的sha1

    std::string compressed_path; // 文件压缩后保存到的路径
};

struct decompress_return
{
    std::string sha1;//解压后的文件的sha1
    std::string decompressed_path;

};


class Compress {
private:
    /* data */
public:
    Compress()=default;
    ~Compress() = default;

    /**
     * 压缩单个文件
     * @param path 文件路径
     * @return compress_return
     */
    compress_return compress(const std::string& path);

    /**
     * 批量压缩文件
     * @param path 文件路径vector<string>
     * @return vector<compress_return>
     */
    std::vector<compress_return> batch_compress(const std::vector<std::string>& path);

    /**
     * 解压单个文件到指定路径
     * @param compressed_path 压缩文件的路径
     * @param path 解压到哪里
     * @return decompress_return
     */
    decompress_return decompress(const std::string &compressed_path,const std::string &path);

    /**
     * 批量解压文件到指定路径
     * @param compressed_path 压缩文件的路径
     * @param path 解压到哪里
     * @return vector<decompress_return>
     */
    std::vector<decompress_return> batch_decompress(const std::vector<std::string> &compressed_path, const std::vector<std::string>&path);


};


#endif //SIMPLE_SCM_COMPRESS_H
