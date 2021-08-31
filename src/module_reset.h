//
// Created by windsoul on 2021/6/11.
//

#ifndef SIMPLE_SCM_MODULE_RESET_H
#define SIMPLE_SCM_MODULE_RESET_H

#include "base_module.h"

class module_reset : public virtual base_module {
public:
    void reset(char []);

    void help() override;
};

#endif //SIMPLE_SCM_MODULE_RESET_H
