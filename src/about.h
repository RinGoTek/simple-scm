//
// Created by login on 2021/9/1.
//

#include<iostream>
using namespace std;
void show_ringo_logo()
{
    cout<<endl
        << "------------     ____________  ------        ----  ---------------      /-------------\\                    \n"
        << "|  |-----|  \\    |___    ___|  |  \\  \\       |  |  | ____________|     /   _________   \\  \n"
        << "|  |     |  |       |    |     |  |\\  \\      |  |  | |                 |   |       |   |            \n"
        << "|  |-----|  |       |    |     |  | \\  \\     |  |  | |                 |   |       |   |            \n"
        << "|    ______/        |    |     |  |  \\  \\    |  |  | |      _________  |   |       |   | \n"
        << "|  |\\  \\            |    |     |  |   \\  \\   |  |  | |      |_____  |  |   |       |   |        \n"
        << "|  |  \\  \\          |    |     |  |    \\  \\  |  |  | |         | || |  |   |       |   |    \n"
        << "|  |    \\  \\     ___|    |___  |  |     \\  \\_|  |  | |________/  || |  \\   |_______|   /                         \n"
        << "|__|      \\__\\   |__________|  |__|      \\______|  |_____________/|_|   \\_____________/                                              \n"
        <<endl;
}
void about()
{
    cout<<"\n\n";
    show_ringo_logo();
    cout<<"\n\n\t\t\tSimple-SCM\n\n"
        <<"\t版本："<<SIMPLE_SCM_VERSION<<endl
        <<"\t版本号："<<SIMPLE_SCM_VERSION_NUMBER<<endl
        <<"\n\tGitHub：  https://github.com/We-are-ACMen/simple-scm"
        <<"\n\t开发者：灵高信息技术"
        <<"\n\tWebsite： http://ringotek.cn"
        <<"\n\tEmail: ringotek@ringotek.cn"
        <<"\n\n\n";
}

