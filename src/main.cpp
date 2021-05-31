#include <iostream>
#include<string>
#include"headers/cache.h"

#include"Database/database.h"
#include"Database/model_commit.h"
#include"Database/model_detect_changes.h"
#include"Database/model_new_branch.h"
#include"Database/model_list.h"

using namespace std;

void deal_with_two_arg(char *parameters[]);//处理有两个参数的函数
void deal_with_three_arg(char *parameters[]);//处理有3个参数的函数

void usage();//输出用法的函数

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
    switch(count)
    {
        case 2:
            deal_with_two_arg(parameters);
            break;
        case 3:
            deal_with_three_arg(parameters);
            break;
        default:
            tip_command_error();
            break;

    }

    return 0;

}

//用于处理2参数指令的函数
void deal_with_two_arg(char *parameters[])
{
    string command = parameters[1];
    if (command == "init")
    {
        //初始化存储库
        database db;
        db.init();
    }
    else if(command == "--usage")
    {
        //输出用法
        usage();

    }
    else if(command == "commit")
    {
        //commit模块
        model_commit tmp;
        tmp.commit();
    }

    else if(command == "list")
    {
        //list模块
        model_list tmp;
        tmp.list();
    }
    else {

        tip_command_error();
    }
}


//用于处理3参数指令的函数
void deal_with_three_arg(char * parameters[])
{
    string command1 = parameters[1];
    char* command2 = parameters[2];

    if (command1 == "new-branch")
    {

        model_new_branch tmp;
        //cout<<const_cast<char *>(command2.c_str())<<endl;
        tmp.create_branch(command2);

    }
    else
        tip_command_error();
}


void usage()
{
    cout<<"Usage:\n"
        <<"init      初始化存储仓库\n"
        <<"new-branch <BranchName>      从当前节点新建分支"
        <<endl;
}