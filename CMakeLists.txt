cmake_minimum_required(VERSION 3.10)

project(shamir VERSION 1.0 DESCRIPTION "shamir")

set(CMAKE_CXX_STANDARD 17)
set(COMPILE_OPT -std=c++17 -O3)

function(filter_items aItems aRegEx)
    foreach (item ${${aItems}})
        if ("${item}" MATCHES ${aRegEx})
            list (REMOVE_ITEM ${aItems} ${item})
        endif ("${item}" MATCHES ${aRegEx})
    endforeach(item)
    set(${aItems} ${${aItems}} PARENT_SCOPE)
endfunction(filter_items)

file(GLOB_RECURSE SHAMIR_SRC
    libs/*.cpp
    include/*.h
)

filter_items(SHAMIR_SRC "CMakeFiles/*")

find_program(CLANG_FORMAT "clang-format-14" REQUIRED)

add_custom_target(format
    COMMAND clang-format-14
    -i
    -style=file
    ${SHAMIR_SRC}
)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(libs)

# TODO(panferovi): rename executable
add_executable(shamir_client shamir/client.cpp)
target_link_libraries(shamir_client PUBLIC server)

add_executable(shamir_server shamir/server.cpp)
target_link_libraries(shamir_server PUBLIC server)

add_executable(math_test test/math_test.cpp)
target_link_libraries(math_test PUBLIC server)
