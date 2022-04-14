file(GLOB SOURCES alglib/*.cpp)
add_library(alglib STATIC ${SOURCES})
target_include_directories(alglib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
