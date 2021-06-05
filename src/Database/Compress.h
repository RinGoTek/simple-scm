//
// Created by login on 2021/6/5.
//

#ifndef SIMPLE_SCM_COMPRESS_H
#define SIMPLE_SCM_COMPRESS_H
#include<string>
#include<vector>


struct compress_return
{

    std::string sha1;   //文件读入后的字符串的sha1
    std::string compressed_path; // 文件压缩后保存到的路径
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



};


#endif //SIMPLE_SCM_COMPRESS_H
