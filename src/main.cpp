#include <iostream>
#include<string>
#include"headers/cache.h"

#include"Database/database.h"
#include"module_commit.h"
#include"module_detect_changes.h"
#include"module_new_branch.h"
#include"module_list.h"
#include "module_ignore.h"

using namespace std;



void usage();//输出用法的函数

//是否是开发模式
bool DEV_MODE = true;

//用来提示命令错误的函数
void tip_command_error()
{
    cout<<"指令错误！"<<endl;
    usage();
    exit(0);
}

//入口函数
int main(int count, char *parameters[]) {
    //用户的命令
    if(count == 1)
    {
        tip_command_error();
        return 0;
    }
    string main_command = parameters[1];
    if (main_command == "init")
    {
        //初始化存储库
        database db;
        db.init();
    }
    else if(main_command == "--usage")
    {
        //输出用法
        usage();

    }
    else if(main_command == "commit")
    {
        //commit模块
        module_commit tmp;
        tmp.commit();
    }

    else if(main_command == "list")
    {
        //list模块
        module_list tmp;

        //列出所有分支的信息
        if(count == 2)
            tmp.list();
    }
    else if (main_command == "new-branch")
    {
        //创建新分支
        module_new_branch tmp;
        //命令错误
        if(count != 3)
            tip_command_error();

        char* command2 = parameters[2];
        tmp.create_branch(command2);

    }
    else if (main_command == "ignore")
    {
        //忽略特定文件
        module_ignore tmp;
        //命令错误
        if(count != 3)
            tip_command_error();

        char* command2 = parameters[2];
        tmp.do_ignore(command2);

    }
    else if(DEV_MODE&&main_command=="walk-folder")
    {
        //walk folder
        //这仅仅是开发用的
        #include "Database/file_system.h"
        auto ans = walk_folder("install");
        for(auto x:ans)
            cout<<x<<endl;
    }
    else if(DEV_MODE&&main_command=="sha1")
    {
        //walk folder
        //这仅仅是开发用的
        #include "Database/file_system.h"
        if(count != 3)
        {
            cerr<<"请输入正确的命令！"<<endl;
            exit(0);
        }
        char* command2 = parameters[2];
        auto ans = calculate_sha1(string(command2));
        cout<<ans<<endl;
    }
    else {

        tip_command_error();
    }

    return 0;

}


void usage()
{
    cout<<"Usage:\n"
        <<"init      初始化存储仓库\n"
        <<"new-branch <BranchName>      从当前节点新建分支\n"
        <<"list      展示当前所有分支\n"
        <<"ignore <path>      添加路径到ignore\n"
        <<endl;
}