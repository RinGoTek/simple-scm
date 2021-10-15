//
// Created by windsoul on 2021/6/2.
//

/*#ifndef SIMPLE_SCM_MODULE_CHECKOUT_H
#define SIMPLE_SCM_MODULE_CHECKOUT_H
class module_checkout{
public:
    void checkout_switch_branch(char[]);
    void checkout_add_switch_branch(char[]);
    void checkout_discard_file_change(char[]);
};
#endif //SIMPLE_SCM_MODULE_CHECKOUT_H
*/
#ifndef SIMPLE_SCM_MODULE_CHECKOUT_H
#define SIMPLE_SCM_MODULE_CHECKOUT_H
#include"base_module.h"
class module_checkout:public virtual base_module{
public:
    module_checkout()
    {
        check_repository_version();
    }
    void checkout_switch_branch(char[]);
    void checkout_switch_node(char[],bool);
    void help() override;

};
#endif //SIMPLE_SCM_MODULE_CHECKOUT_H