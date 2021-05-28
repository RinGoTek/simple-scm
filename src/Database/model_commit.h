//
// Created by a2447742618 on 2021/5/28.
//

#ifndef SIMPLE_SCM_MODEL_COMMIT_H
#define SIMPLE_SCM_MODEL_COMMIT_H
#include"model_detect_changes.h"


class model_commit {
    friend class model_detect_changes;

    public:
        void commit();
};


#endif //SIMPLE_SCM_MODEL_COMMIT_H
