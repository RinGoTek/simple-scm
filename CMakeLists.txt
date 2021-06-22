cmake_minimum_required(VERSION 3.19)
project(simple-scm)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)


add_executable(simple-scm src/main.cpp src/headers/global.h src/Database/init-db.cpp src/Database/init-db.h src/Database/database.cpp src/Database/database.h src/module_detect_changes.h src/module_detect_changes.cpp src/module_detect_changes.cpp src/module_detect_changes.h src/module_commit.cpp src/module_commit.h src/module_new_branch.cpp src/module_new_branch.h src/module_list.cpp src/module_list.h src/Database/file_system.cpp src/Database/file_system.h src/module_ignore.cpp src/module_ignore.h src/module_add.cpp src/module_add.h src/module_checkout.cpp src/module_checkout.h src/Database/Compress.cpp src/Database/Compress.h src/libs/bundle/bundle.h src/libs/bundle/bundle.cpp src/module_merge.cpp src/module_merge.h src/module_reset.cpp src/module_reset.h src/Test/test_compress.cpp src/Test/test_compress.h)
# 解决bundle无法编译的问题
target_link_libraries(simple-scm -lpthread -lm)

target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)

add_compile_options(-l sqlite3)
target_link_libraries(${PROJECT_NAME} sqlite3)


set(CMAKE_INSTALL_PREFIX ./install)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${PROJECT_NAME}/bin)
