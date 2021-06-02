//
// Created by a2447742618 on 2021/5/28.
//

#ifndef SIMPLE_SCM_MODULE_DETECT_CHANGES_H
#define SIMPLE_SCM_MODULE_DETECT_CHANGES_H

#include<map>
#include<vector>

using namespace std;

class module_detect_changes {
public:
    module_detect_changes();

    void detect_changes();

    //map <string,char*> object_info; //储存从文件哈希值到文件更改时间的映射
    map<string, bool> vis;
    vector<string> object;
};


#endif //SIMPLE_SCM_MODULE_DETECT_CHANGES_H