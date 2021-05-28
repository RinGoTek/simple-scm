//
// Created by a2447742618 on 2021/5/28.
//

#include "model_commit.h"
#include "model_detect_changes.h"
#include<iostream>
using namespace std;

void model_commit::commit()
{
    model_detect_changes tmp;
    tmp.detect_changes();
}
