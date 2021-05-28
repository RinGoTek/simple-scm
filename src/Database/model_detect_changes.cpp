//
// Created by a2447742618 on 2021/5/28.
//

#include "model_detect_changes.h"
#include<sqlite3.h>
#include<string>
#include<iostream>
using namespace std;


void detect_changes()
{
    extern int current_head;
    string head_node="null";

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char **result;
    int row, column;

    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if( rc )
    {
        clog<<"[ERROR]数据库打开失败："<<endl;
        exit(1);
    }
    else
    {
        clog<<"[INFO]数据库打开成功！"<<endl;
    }

    rc = sqlite3_get_table(db, "SELECT * FROM Branch", &result, &row, &column, &zErrMsg);

    if(rc != SQLITE_OK)
    {
        clog<<"[ERROR]数据表信息获取失败："<<zErrMsg<<endl;
    }
    else
    {
        for(int i=1;i<=row;i++)
        {
            if(result[i*column][0] == current_head)
            {
                head_node = result[i*column][3];
                clog<<"[INFO]头节点获取成功！"<<endl;
                cout<<head_node<<endl;
                break;
            }
        }
    }
    if(head_node == "null") clog<<"[ERROR]找不到头节点"<<endl;
}