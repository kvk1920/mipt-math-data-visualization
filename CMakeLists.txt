cmake_minimum_required(VERSION 3.16)
project(datavis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


include(FetchContent)

FetchContent_Declare(
        pugixml
        GIT_REPOSITORY https://github.com/zeux/pugixml.git
        GIT_TAG v1.12.1
)

FetchContent_MakeAvailable(pugixml)

add_subdirectory(extern)
add_subdirectory(src)
add_subdirectory(apps)
