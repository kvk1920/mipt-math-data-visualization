add_library(datavis STATIC
        datavis/graphml.cpp
        datavis/svg.cpp)
target_link_libraries(datavis PRIVATE pugixml)
target_include_directories(datavis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
