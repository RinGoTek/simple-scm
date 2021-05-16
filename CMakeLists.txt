cmake_minimum_required(VERSION 3.19)
project(simple-scm)

set(CMAKE_CXX_STANDARD 17)
find_package(ZLIB)
find_package(OpenSSL REQUIRED)


add_executable(simple-scm src/main.cpp src/headers/cache.h src/init-db.cpp src/headers/init-db.h)

target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)

set(CMAKE_INSTALL_PREFIX ./install)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${PROJECT_NAME}/bin)
