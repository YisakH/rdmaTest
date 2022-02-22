
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/memory)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/queues)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/requests)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)

add_library(infinity STATIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(infinity PRIVATE core memory queues requests utils)