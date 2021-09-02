//
// Created by login on 2021/9/1.
//

#ifndef SIMPLE_SCM_UPDATE_H
#define SIMPLE_SCM_UPDATE_H



using namespace std;
//该部分代码使用python3编写
void update()
{
    string simple_scm_path = GET_SIMPLE_SCM_PATH();
    system(("python "+ simple_scm_path+"PythonFiles/update.py").c_str());
}

#endif //SIMPLE_SCM_UPDATE_H
