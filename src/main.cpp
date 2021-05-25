#include <iostream>
#include<string>
#include"headers/cache.h"

#include"Database/database.h"


using namespace std;

void deal_with_two_arg(char *parameters[]);//处理有两个参数的函数

void usage();//输出用法的函数

//入口函数
int main(int count, char *parameters[]) {
    //用户的命令
    if(count == 1)
    {
        cout<<"请使用 --usage 来查看用法."<<endl;
        return 0;
    }
    switch(count)
    {
        case 2:
            deal_with_two_arg(parameters);
            break;
        default:
            usage();
            break;

    }

}

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
    else {

        cout << "请输入正确的命令"<<endl;
        usage();
    }
}


void usage()
{
    cout<<"Usage:\n"
        <<"init      初始化存储仓库\n"
        <<endl;
}