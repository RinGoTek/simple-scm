//
// Created by a2447742618 on 2021/5/28.
//

#ifndef SIMPLE_SCM_MODEL_DETECT_CHANGES_H
#define SIMPLE_SCM_MODEL_DETECT_CHANGES_H
#include"model_commit.h"
#include<map>
using namespace std;

class model_detect_changes {
    friend class model_commit;

    public:
        static void detect_changes();

        static pair <string,char*> table_info;
};


#endif //SIMPLE_SCM_MODEL_DETECT_CHANGES_H
