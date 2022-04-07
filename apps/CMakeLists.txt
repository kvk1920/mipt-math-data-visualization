
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(vis-tree vis-tree.cpp)
target_link_libraries(vis-tree PRIVATE datavis)
