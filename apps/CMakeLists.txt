
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(vis-tree vis-tree.cpp)
target_link_libraries(vis-tree PRIVATE datavis)

add_executable(vis-dag vis-dag.cpp)
target_link_libraries(vis-dag PRIVATE datavis alglib)

add_executable(vis-labels vis-labels.cpp)
target_link_libraries(vis-labels PRIVATE datavis)
