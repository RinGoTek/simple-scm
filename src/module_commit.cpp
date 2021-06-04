//
// Created by a2447742618 on 2021/5/28.
//

#include "module_commit.h"
#include "module_detect_changes.h"
#include<iostream>

using namespace std;

void module_commit::commit() {

    detect_info info;
    module_detect_changes commit_tmp;
    info = commit_tmp.detect_changes();


}
