cmake_minimum_required(VERSION 3.19)
project(simple-scm)

set(CMAKE_CXX_STANDARD 17)
find_package(ZLIB)
find_package(OpenSSL REQUIRED)


add_executable(simple-scm src/main.cpp src/headers/cache.h src/Database/init-db.cpp src/Database/init-db.h src/Database/database.cpp src/Database/database.h src/module_detect_changes.h src/module_detect_changes.cpp src/module_detect_changes.cpp src/module_detect_changes.h src/module_commit.cpp src/module_commit.h src/module_new_branch.cpp src/module_new_branch.h src/module_list.cpp src/module_list.h src/Database/file_system.cpp src/Database/file_system.h src/module_add.cpp src/module_add.h)

target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)

add_compile_options(-l sqlite3)
target_link_libraries(${PROJECT_NAME} sqlite3)


set(CMAKE_INSTALL_PREFIX ./install)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${PROJECT_NAME}/bin)
