cmake_minimum_required(VERSION 3.19)
project(simple-scm)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)


add_executable(simple-scm src/main.cpp src/headers/global.h src/Database/init-db.cpp src/Database/init-db.h src/Database/database.cpp src/Database/database.h src/module_detect_changes.h src/module_detect_changes.cpp src/module_detect_changes.cpp src/module_detect_changes.h src/module_commit.cpp src/module_commit.h src/module_new_branch.cpp src/module_new_branch.h src/module_list.cpp src/module_list.h src/Database/file_system.cpp src/Database/file_system.h src/module_ignore.cpp src/module_ignore.h src/module_add.cpp src/module_add.h src/module_checkout.cpp src/module_checkout.h src/Database/Compress.cpp src/Database/Compress.h src/libs/bundle/bundle.h src/libs/bundle/bundle.cpp src/module_merge.cpp src/module_merge.h src/module_reset.cpp src/module_reset.h src/Test/test_compress.cpp src/Test/test_compress.h src/Test/test.cpp src/Test/test.h src/Test/test_database.cpp src/Test/test_database.h src/Test/test_file_system.cpp src/Test/test_file_system.h src/Test/test_commit.cpp src/Test/test_commit.h src/Test/test_add.cpp src/Test/test_add.h src/Test/test_checkout.cpp src/Test/test_checkout.h src/Test/test_reset.cpp src/Test/test_reset.h src/Test/test_merge.cpp src/Test/test_merge.h src/module_edit.cpp src/module_edit.h src/base_module.cpp src/base_module.h src/migrate/migrate.cpp src/migrate/migrate.h src/about.h src/update.h)
# 解决bundle无法编译的问题
target_link_libraries(simple-scm -lpthread -lm)

target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)

add_compile_options(-l sqlite3)
target_link_libraries(${PROJECT_NAME} sqlite3)


set(CMAKE_INSTALL_PREFIX ./install)
set(CMAKE_CXX_FLAGS_RELEASE " -O3")

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${PROJECT_NAME}/bin)
install(DIRECTORY src/PythonFiles DESTINATION ${PROJECT_NAME}/bin)
install(DIRECTORY static_files/ DESTINATION ${PROJECT_NAME}/static)
