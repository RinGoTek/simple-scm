//
// Created by login on 2021/6/22.
//

#include "test_database.h"
#include<iostream>
#include<ctime>
#include"../Database/database.h"
using namespace std;

void test_database::test_get_current_time_char() {
    cout<<database::getCurrentTimeChar()<<endl;
}

void test_database::test_get_time_char() {
    time_t tt;
    time(&tt);
    cout<<database::getTimeChar(tt)<<endl;

}
