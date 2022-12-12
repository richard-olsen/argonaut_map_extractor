cmake_minimum_required(VERSION 3.2)

project(argoMapExtractor CXX)

add_executable(argoMapExtractor "main.cpp" "bigb/bigb_util.cpp" "image/image.h" "image/image.cpp" "util/stb_image_write.h" "audio/audio.h" "audio/audio.cpp" "model/vertex.h" "model/model.h" "model/model.cpp" "audio/imadecoder.h" "audio/imadecoder.cpp")