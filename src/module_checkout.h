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

class module_checkout{
public:
    void checkout_switch_branch(char[]);
    void checkout_switch_node(char[]);
    void checkout_add_switch_branch(char[]);
    void checkout_discard_file_change(char[]);
};
#endif //SIMPLE_SCM_MODULE_CHECKOUT_H