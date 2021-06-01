//
// Created by a2447742618 on 2021/5/28.
//

#include "module_commit.h"
#include "module_detect_changes.h"
#include<iostream>

using namespace std;

void module_commit::commit() {
    module_detect_changes tmp;
    tmp.detect_changes();
}
