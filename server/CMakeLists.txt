cmake_minimum_required(VERSION 3.11)

project(
    SocksManyToMany
    VERSION 0.1
    DESCRIPTION "다대다 통신 프로젝트"
    LANGUAGES CXX
)

#set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_BUILD_TYPE Debug)
#find_package(Threads REQUIRED)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/my)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/tools)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/network)

add_executable(program ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

#add_subdirectory(lib)
#target_link_libraries(program PRIVATE Threads::Threads)
target_link_libraries(program PRIVATE pthread ibverbs my tools network)
target_include_directories(program PUBLIC includes)