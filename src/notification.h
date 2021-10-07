//
// Created by longjin on 2021/10/7.
//
//这是封装输出信息的头文件

#ifndef SIMPLE_SCM_NOTIFICATION_H
#define SIMPLE_SCM_NOTIFICATION_H

#include"string"
#include<iostream>
#include "cstdio"

static void print(char *prev, const char *msg) {
    std::printf("[%s] %s\n", prev, msg);
}

static void info(const std::string& msg) {
    print("INFO", msg.c_str());
}

static void info(char *msg) {
    print("INFO", msg);
}

static void error(const std::string &msg) {
    print("ERROR", msg.c_str());
}

static void error(char *msg) {
    print("ERROR", msg);
}

static void warning(const std::string &msg) {
    print("WARNING", msg.c_str());
}

static void warning(char *msg) {
    print("WARNING", msg);
}

static void success(const std::string &msg) {
    print("SUCCESS", msg.c_str());
}

static void success(char *msg) {
    print("SUCCESS", msg);
}


#endif //SIMPLE_SCM_NOTIFICATION_H
