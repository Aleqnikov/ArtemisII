cmake_minimum_required(VERSION 3.14)
project(ArtemisII)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG 0.8.0
)

FetchContent_Declare(
        cli11
        URL https://github.com/CLIUtils/CLI11/releases/download/v2.4.1/CLI11.hpp
        DOWNLOAD_NO_EXTRACT TRUE
)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(yaml-cpp cli11)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
list(FILTER SOURCES EXCLUDE REGEX ".*/_deps/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*/CMakeFiles/.*")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        .
        ${cli11_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()