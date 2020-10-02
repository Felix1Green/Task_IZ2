cmake_minimum_required(VERSION 3.1)
project(IZ2)

add_definitions(-DPATH_TO_FILE="${PROJECT_SOURCE_DIR}/file.txt")
add_definitions(-DPATH_TO_DYN_LIB="${PROJECT_SOURCE_DIR}/build/libdynamic_lib.so")

#  GTest
configure_file(CMakeLists.txt.in
        googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build)

enable_testing()
add_subdirectory(test)


set(CMAKE_C_FLAGS "-Wall -Werror -Wpedantic")
set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/dynamic_library ${PROJECT_SOURCE_DIR}/static_library)


add_library(static_lib STATIC static_library/lib_func.c)
add_library(dynamic_lib SHARED dynamic_library/libd_func.c)

add_executable(dynamic_main src/dynamic_main.c src/func.c)
add_executable(static_main src/static_main.c src/func.c)

target_link_libraries(dynamic_lib pthread)

target_link_libraries(dynamic_main dynamic_lib dl)
target_link_libraries(static_main static_lib)