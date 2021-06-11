//
// Created by a2447742618 on 2021/5/28.
//

#ifndef SIMPLE_SCM_MODULE_DETECT_CHANGES_H
#define SIMPLE_SCM_MODULE_DETECT_CHANGES_H

#include<map>
#include<vector>

using namespace std;

struct detect_info//用来保存检测结果
{
    vector<string> del;//保存删除的文件哈希值
    vector<string> change;//保存更改的文件哈希值
    map<string,string> path2SHA;//保存文件路径到CompressedSHA的映射，用于del和change

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

struct node_info
{
    string compressed_sha;
    string origin_path;
    string compressed_path;
    string updated_datetime;
    string created_datetime;
    string origin_sha;

    node_info()
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

    detect_info detect_changes();

    vector<node_info> get_node_info(string);
};


#endif //SIMPLE_SCM_MODULE_DETECT_CHANGES_H
