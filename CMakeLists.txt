cmake_minimum_required(VERSION 3.10)

project(shamir VERSION 1.0 DESCRIPTION "shamir")

set(CMAKE_CXX_STANDARD 17)
set(COMPILE_OPT -std=c++17 -O3)

# Remove strings matching given regular expression from a list.
# @param(in,out) aItems Reference of a list variable to filter.
# @param aRegEx Value of regular expression to match.
function (filter_items aItems aRegEx)
    # For each item in our list
    foreach (item ${${aItems}})
        # Check if our items matches our regular expression
        if ("${item}" MATCHES ${aRegEx})
            # Remove current item from our list
            list (REMOVE_ITEM ${aItems} ${item})
        endif ("${item}" MATCHES ${aRegEx})
    endforeach(item)
    # Provide output parameter
    set(${aItems} ${${aItems}} PARENT_SCOPE)
endfunction (filter_items)

file(GLOB_RECURSE SHAMIR_SRC
    src/*.cpp
    include/*.h
)

filter_items(SHAMIR_SRC "CMakeFiles/*")

find_program(CLANG_FORMAT "clang-format" REQUIRED)

add_custom_target(format
    COMMAND clang-format
    -i
    -style=file
    ${SHAMIR_SRC}
)

