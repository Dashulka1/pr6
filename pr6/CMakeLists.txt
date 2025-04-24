cmake_minimum_required(VERSION 3.10)
project(search_barrier)

find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

add_executable(task1 task1.c)
add_executable(task2   task2.c)
