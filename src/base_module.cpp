//
// Created by login on 2021/8/31.
//

#include "base_module.h"

void base_module::check_repository_version() {
    //检验存储库版本，旧版则需要更新版本
    std::ifstream fin(".simple-scm/VERSION");
    if (!fin) {
        //旧版本的simple-scm没有这个文件，这个文件是版本号为3时加入的
        std::cerr << "[WARNING]当前存储库是旧版本的，您必须对其进行迁移，方能继续使用它" << std::endl;
        exit(0);
    }
    std::string str;
    fin >> str;
    std::stringstream ss;
    ss << str;
    int ver;
    ss >> ver;
    if (ver < SIMPLE_SCM_VERSION_NUMBER) {
        std::cerr << "[WARNING]当前存储库是旧版本的，您必须对其进行迁移，方能继续使用它" << std::endl;
        exit(0);
    }

}
