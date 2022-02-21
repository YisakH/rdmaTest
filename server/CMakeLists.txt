cmake_minimum_required(VERSION 3.11)

project(
    SocksManyToMany
    VERSION 0.1
    DESCRIPTION "다대다 통신 프로젝트"
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/my)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/tools)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/infinity)


file( GLOB APP_SOURCES ${CMAKE_SOURCE_DIR}/*.cpp )

message(STATUS ${APP_SOURCES})

foreach( testsourcefile ${APP_SOURCES} )
    # I used a simple string replace, to cut off .cpp.
    string( REPLACE ".cpp" "_exe" testname ${testsourcefile})
    string( REPLACE ${CMAKE_SOURCE_DIR}/ "" buildname ${testname})
    add_executable( ${buildname} ${testsourcefile})
    # Make sure YourLib is linked to each app
    target_link_libraries( ${buildname} PRIVATE pthread ibverbs my tools infinity)
    target_include_directories(${buildname} PUBLIC ${CMAKE_SOURCE_DIR}/include)
endforeach( testsourcefile ${APP_SOURCES} )