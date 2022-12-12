cmake_minimum_required(VERSION 3.2)

project(ArgonautExtractor CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/common/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")

add_subdirectory(voltExtractor)
add_subdirectory(voltPacker)
add_subdirectory(argoMapExtractor)
