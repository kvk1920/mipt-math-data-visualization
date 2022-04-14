#file(GLOB SOURCES alglib/*.cpp)

# NB: We don't need to compile whole alglib here.
set(SOURCES
        alglib/optimization.cpp
        alglib/alglibmisc.cpp
        alglib/ap.cpp
        alglib/alglibinternal.cpp
        alglib/linalg.cpp)
add_library(alglib STATIC ${SOURCES})
target_include_directories(alglib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
