file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)


add_library(utils STATIC ${SRC_FILES})

target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include/infinity/utils)