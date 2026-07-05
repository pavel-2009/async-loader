cmake_minimum_required(VERSION 3.20)

project(async_loader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(async_loader
    main.cpp
    src/async_loader.cpp
    src/thread_pool.cpp
)

target_include_directories(async_loader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(async_loader PRIVATE Threads::Threads)