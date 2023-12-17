cmake_minimum_required(VERSION 3.10)

project(shagit VERSION 1.0 DESCRIPTION "shagit")

set(SERVER_SRC
    server/client.cpp
    server/tcp_server.cpp
    threadpool/thread_pool.cpp
    math/math.cpp
    storage/storage.cpp
)

set(BOOST_LIB -lboost_system)

add_library(server ${SERVER_SRC})

target_include_directories(server
    PUBLIC ${INCLUDE_DIR}
)

target_link_libraries(server PUBLIC
    ${BOOST_LIB}
    utils
)

set(UTILS_SRC
    utils/debug.cpp
)

add_library(utils ${UTILS_SRC})

target_include_directories(utils
    PUBLIC ${INCLUDE_DIR}
)

