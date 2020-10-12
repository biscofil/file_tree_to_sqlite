cmake_minimum_required(VERSION 3.16)
project(folderList)

set(CMAKE_CXX_STANDARD 17)

find_package (SQLite3 REQUIRED)
add_compile_options(-l sqlite3)

add_executable(folderList src/main.cpp src/sqliteManager.h)

include_directories(${SQLITE3_INCLUDE_DIRS})
target_link_libraries (folderList LINK_PUBLIC sqlite3)
