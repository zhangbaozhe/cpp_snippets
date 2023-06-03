cmake_minimum_required(VERSION 3.15.2)
project(cpp_snippet)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")

set(SRC 
  ${PROJECT_SOURCE_DIR}/src/generic_timeout_function.cpp
  ${PROJECT_SOURCE_DIR}/src/safe_read_write.cpp
)

foreach(cpp_file ${SRC})
  get_filename_component(target ${cpp_file} NAME_WE)
  message("The source file is \n\t${cpp_file},\ntarget is \n\t${target}")
  add_executable(${target} ${cpp_file})
endforeach()
