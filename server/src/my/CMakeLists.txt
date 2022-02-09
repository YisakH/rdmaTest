file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_library(my STATIC ${SRC_FILES})


target_include_directories(my PUBLIC ../../include/my)

target_link_libraries(my PRIVATE pthread ibverbs)


