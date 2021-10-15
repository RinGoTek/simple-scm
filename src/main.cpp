#include <iostream>
#include<string>
#include"headers/global.h"

#include"Database/database.h"
#include"module_commit.h"
#include"module_detect_changes.h"
#include"module_new_branch.h"
#include"module_list.h"
#include "module_reset.h"
#include "module_ignore.h"
#include"module_add.h"
#include "module_checkout.h"
#include "Database/file_system.h"
#include "Database/Compress.h"
#include "module_edit.h"
#include"module_merge.h"

#include "migrate/migrate.h"
#include "about.h"
#include "update.h"

#include"Test/test.h"



using namespace std;


void usage();//输出用法的函数


//用来提示命令错误的函数
void tip_command_error() {
    cout << "指令错误！" << endl;
    usage();
    exit(0);
}

//入口函数
int main(int count, char *parameters[]) {
    //用户的命令
    if (count == 1) {
        tip_command_error();
        return 0;
    }
    string main_command = parameters[1];
    if (main_command == "init") {
        //初始化存储库
        database db;
        db.init();
    } else if (main_command == "--usage") {
        //输出用法
        usage();

    } else if (main_command == "commit") {
        //commit模块
        module_commit tmp;

        if (count != 3)
            tip_command_error();

        char *command2 = parameters[2];
        tmp.commit(command2);
    } else if (main_command == "list") {
        //list模块
        module_list tmp;

        //列出所有分支的信息
        if (count == 2)
            tmp.list();
    } else if (main_command == "new-branch") {
        //创建新分支
        module_new_branch tmp;
        //命令错误
        if (count != 3)
            tip_command_error();

        char *command2 = parameters[2];
        tmp.create_branch(command2);

    } else if (main_command == "add") {
        module_add tmp;
        if (count != 3)
            tip_command_error();

        char *command2 = parameters[2];
        tmp.add(command2);
    } else if (main_command == "ignore") {
        //忽略特定文件
        module_ignore tmp;

        if (count == 3) {
            //添加到ignore
            char *command2 = parameters[2];
            tmp.do_ignore(command2);
        } else if (count == 4) {
            string command2 = parameters[2];
            char *command3 = parameters[3];

            //取消ignore
            if (command2 == "-d") {
                tmp.de_ignore(command3);
            }
        } else tip_command_error();

    } else if (main_command == "merge") {
        //将另一个节点并入当前分支
        if (count != 3)
            tip_command_error();
        module_merge tmp;
        tmp.merge(parameters[2]);
    } else if (DEV_MODE && main_command == "test") {
        test tmp;
        tmp.start_test();
    } else if (DEV_MODE && main_command == "walk-folder") {
        //walk folder
        //这仅仅是开发用的


        auto ans = walk_folder(".");

        for (auto x:ans)
            cout << x << endl;
    } else if (DEV_MODE && main_command == "sha1") {

        //这仅仅是开发用的
        cout << "xxx" << endl;
        cout << calculate_char_sha1("123456") << endl;
        cout << calculate_string_sha1("123456") << endl;


        if (count != 3) {
            cerr << "请输入正确的命令！" << endl;
            exit(0);
        }

        char *command2 = parameters[2];
        auto ans = calculate_sha1(string(command2));
        cout << ans << endl;
    } else if (main_command == "checkout") {
        string command2 = parameters[2];
        module_checkout ck;

        if (command2 == "-n") {
            //切换到节点
            ck.checkout_switch_node(parameters[3],true);
        } else {
            //切换分支
            ck.checkout_switch_branch(const_cast<char *>(command2.c_str()));
        }


    } else if (main_command == "reset") {
        char *command2 = parameters[2];
        module_reset rbq;
        rbq.reset(command2);
    } else if (main_command == "edit") {
        //编辑信息
        module_edit tmp;
        char *tmp_para[100];
        for(int i=2;i<count;++i)
            tmp_para[i-2] = parameters[i];
        tmp.edit(tmp_para, count-2);

    }
    else if(main_command=="-v"||main_command=="-V")
    {
        cout<<"当前版本名："<<SIMPLE_SCM_VERSION<<endl;
        cout<<"当前版本号："<<SIMPLE_SCM_VERSION_NUMBER<<endl;
    }
    else if (main_command == "migrate") {
        //数据库迁移
        migrate tmp;
        tmp.action();

    }
    else if (main_command == "about") {
        //关于界面
        about();
    }
    else if (main_command == "update") {
        //升级软件
        update();
    }
    else {

        tip_command_error();
    }
    return 0;
}


void usage() {
    cout << "Usage:\n"
         << "init                       初始化存储仓库\n"
         << "add <path>                 添加路径到AddList\n"
         << "new-branch <BranchName>    从当前节点新建分支\n"
         << "commit <Message>           提交代码到存储库\n"
         << "list                       展示当前所有分支\n"
         << "ignore <path>              添加路径到ignore\n"
         << "ignore -d <path>           取消ignore\n"
         << "checkout <BranchName>      切换分支\n"
         << "checkout -n <SHA1>         切换到指定节点\n"
         << "reset <SHA1>               回退当前分支的版本\n"
         << "merge <SHA1>               将另一个节点合并入当前分支的头节点\n"
         << "edit -n <SHA1> -t <TEXT>   编辑提交文本\n"
         << "migrate                    迁移存储库版本（当存储库版本过低时）\n"
         << "-V                         查询当前版本号\n"
         << "about                      显示软件信息\n"
         << "update                     检查软件更新\n"
         << endl;
}