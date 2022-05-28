cmake_minimum_required(VERSION 3.20)
project(test C)

set(CMAKE_C_STANDARD 99)

add_executable(test main.c)

include_directories(C:/cygwin64/usr/include)
find_library(GLPK glpk)
message(STATUS, "glpk is ${GLPK}")
target_link_libraries(test ${GLPK})