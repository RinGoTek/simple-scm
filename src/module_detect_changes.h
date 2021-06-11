//
// Created by a2447742618 on 2021/5/28.
//

#ifndef SIMPLE_SCM_MODULE_DETECT_CHANGES_H
#define SIMPLE_SCM_MODULE_DETECT_CHANGES_H

#include<map>
#include<vector>



struct detect_info//用来保存检测结果
{
    std::vector<std::string> del;//保存删除的文件哈希值
    std::vector<std::string> change;//保存更改的文件哈希值
    std::map<std::string,std::string> path2SHA;//保存文件路径到CompressedSHA的映射，用于del和change

    detect_info() {
        del.clear();
        change.clear();
        path2SHA.clear();
    }

    detect_info &operator=(const detect_info &tmp) {
        path2SHA=tmp.path2SHA;
        del = tmp.del;
        change = tmp.change;
        return *this;
    }

};

struct file_info
{
    std::string compressed_sha;
    std::string origin_path;
    std::string compressed_path;
    std::string updated_datetime;
    std::string created_datetime;
    std::string origin_sha;

    file_info()
    {
        compressed_path="";
        compressed_sha="";
        origin_path="";
        updated_datetime="";
        created_datetime="";
        origin_sha="";
    }
};

class module_detect_changes {
public:
    module_detect_changes() {};

    /**
     * 检测指定节点与当前文件系统中文件的改变情况
     * @param NodeSHA 指定的节点的sha
     * @return detect_info
     */
    detect_info detect_changes(std::string NodeSHA);

    /**
     * 获取节点的文件列表的函数
     * @param NodeSHA 节点的sha1
     * @return vector<node_info>
     */
    std::vector<file_info> get_node_files(std::string NodeSHA);
};


#endif //SIMPLE_SCM_MODULE_DETECT_CHANGES_H
