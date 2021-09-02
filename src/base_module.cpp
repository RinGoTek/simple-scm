//
// Created by login on 2021/8/31.
//

#include "base_module.h"

using namespace std;

void base_module::check_repository_version() {
    //检验存储库版本，旧版则需要更新版本
    std::ifstream fin(".simple-scm/VERSION");
    if (!fin) {
        //旧版本的simple-scm没有这个文件，这个文件是版本号为3时加入的
        std::cerr << "[WARNING]当前存储库是旧版本的，您必须对其进行迁移，方能继续使用它.请输入simple-scm migrate以迁移存储库" << std::endl;
        exit(0);
    }
    std::string str;
    fin >> str;
    std::stringstream ss;
    ss << str;
    int ver;
    ss >> ver;
    if (ver < SIMPLE_SCM_VERSION_NUMBER) {
        std::cerr << "[WARNING]当前存储库是旧版本的，您必须对其进行迁移，方能继续使用它.请输入simple-scm migrate以迁移存储库" << std::endl;
        exit(0);
    } else if (ver > SIMPLE_SCM_VERSION_NUMBER) {
        std::cerr << "[WARNING]当前存储库是新版本的，若您要继续使用，请升级Simple-SCM。" << endl;
        exit(0);
    }

}

static void tip_cmd_error() {
    cerr << "指令不合法！" << endl;
    exit(0);
}

std::vector<std::pair<std::string, std::string> > base_module::parse_ops(char **parameters, int size) {
    vector<pair<string, string> > ops;//存储操作
    if (parameters[0][0] != '-')
        tip_cmd_error();

    for (int i = 0; i < size; ++i) {
        string txt = parameters[i];
        if (txt[0] == '-') {
            if ((i != size - 1) && parameters[i + 1][0] != '-') {
                ops.emplace_back(make_pair(txt, parameters[i + 1]));
                ++i;
                continue;
            } else if ((i != size - 1) && parameters[i + 1][0] == '-') {
                //不能连续两个 -开头的
                tip_cmd_error();
            }

        }
    }

    return ops;
}
