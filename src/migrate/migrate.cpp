//
// Created by login on 2021/9/1.
//

#include "migrate.h"
#include "iostream"
#include "fstream"
#include "sstream"
#include "sqlite3.h"
#include "vector"
#include "../Database/database.h"

using namespace std;


void migrate::migrate_to_3() {

    //开启事务
    sprintf(sql, "BEGIN;");
    database::exec_with_transaction(db, sql, zErrMsg, 0);

    //node新增Editable字段
    sprintf(sql, "ALTER TABLE Node ADD COLUMN Editable BOOLEAN NOT NULL DEFAULT TRUE");
    database::exec_with_transaction(db, sql, zErrMsg, 0);

    //把原有某些节点设为不可更改文本

    sprintf(sql, "UPDATE Node SET Editable = FALSE WHERE Message LIKE 'Merge%%'");

    database::exec_with_transaction(db, sql, zErrMsg, 0);
    sprintf(sql, "UPDATE Node SET Editable = FALSE WHERE SHA = '000000'");
    database::exec_with_transaction(db, sql, zErrMsg, 0);


    //提交事务
    sprintf(sql, "COMMIT;");
    database::exec_with_transaction(db, sql, zErrMsg, 0);

    std::ofstream fout(".simple-scm/VERSION");
    fout << 3;
    fout.close();
    ver = 3;
}

bool migrate::check_version() {
    //检验存储库版本，旧版则需要更新版本
    std::ifstream fin(".simple-scm/VERSION");
    if (!fin) {
        //旧版本的simple-scm没有这个文件，这个文件是版本号为3时加入的
        ver = 1;
        return true;
    }
    std::string str;
    fin >> str;
    fin.close();
    std::stringstream ss;
    ss << str;

    ss >> ver;
    if (ver < SIMPLE_SCM_VERSION_NUMBER) {
        return true;
    } else if (ver > SIMPLE_SCM_VERSION_NUMBER) {
        std::cerr << "[WARNING]当前存储库是新版本的，当前版本的Simple-SCM无法支持。若您要继续使用，请升级Simple-SCM软件。" << endl;
        exit(0);
    } else {
        cout << "存储库已经是最新版本的了！" << endl;
        return false;
    }
}

void migrate::action() {
    //打开数据库
    rc = sqlite3_open(".simple-scm/simple-scm.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "[ERROR]数据库打开失败：" << zErrMsg << endl;
        exit(1);
    } else {
        if (DEV_MODE)
            clog << "[INFO]数据库打开成功！" << endl;
    }
    if (!check_version())
        exit(0);
    printf("您确定要把该存储库从 版本%d 升级到 版本%d 吗？\n", ver, SIMPLE_SCM_VERSION_NUMBER);
    cout << "在升级前，您应当知晓并深刻理解：\n"
         << "1、升级是单向的过程，无法回退，您的伙伴若使用较低版本的Simple-SCM，他们需要升级软件版本才能继续使用！\n"
         << "2、升级过程可能有风险，请备份好您的数据！对于数据丢失，我们不负责任。\n"
         << "3、升级可能需要一点时间，请不要中途退出！否则可能导致存储库损坏！\n";
    bool flag = true;
    string s;
    while (true) {
        cout << "我认真阅读了以上文字，确定升级，并愿意承担由此带来的一切风险. (Y/N)>>";
        cin >> s;
        if (s[0] == 'y' || s[0] == 'Y') {
            break;
        } else if (s[0] == 'n' || s[0] == 'N') {
            flag = false;
            break;
        }
    }

    if (flag) {
        cout << "正在升级..." << endl;
        migrate_to_3();
        cout << "存储库升级成功！" << endl;
    } else {
        cout << "已取消操作！" << endl;
        exit(0);
    }
}
