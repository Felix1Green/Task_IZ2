file(GLOB sources "${PROJECT_SOURCE_DIR}/src/*.c")
list(REMOVE_ITEM sources "${PROJECT_SOURCE_DIR}/src/dynamic_main.c")
list(REMOVE_ITEM sources "${PROJECT_SOURCE_DIR}/src/static_main.c")

file(GLOB tests "${PROJECT_SOURCE_DIR}/test/*.cpp")
list(REMOVE_ITEM tests "${PROJECT_SOURCE_DIR}/test/main.cpp")

add_definitions(-DPATH_TO_FILE="${PROJECT_SOURCE_DIR}/file.txt")
add_definitions(-DPATH_TO_STFILE_TEST="${PROJECT_SOURCE_DIR}/test/files/static_file_test.txt")
add_definitions(-DPATH_TO_STBIGGER_TEST="${PROJECT_SOURCE_DIR}/test/files/bigger_string.txt")
add_definitions(-DPATH_TO_BIGFILE_TEST="${PROJECT_SOURCE_DIR}/test/files/big_file.txt")

include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/static_library"
        "${PROJECT_SOURCE_DIR}/dynamic_library")


foreach(file ${tests})
    set(name)
    get_filename_component(name ${file} NAME_WE)
    add_executable("${name}_tests"
            ${sources}
            ${file}
            "${PROJECT_SOURCE_DIR}/test/main.cpp")
    target_link_libraries("${name}_tests" gtest_main static_lib dynamic_lib dl)
    set (CMAKE_CXX_STANDARD 11)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage -Wall -Werror -Wpedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    add_test(NAME ${name} COMMAND "${name}_tests")
endforeach()