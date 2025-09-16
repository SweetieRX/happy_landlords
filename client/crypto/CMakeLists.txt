cmake_minimum_required(VERSION 3.16.3)
project(crypto)

aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(crypto ${SRC} ${HEADERS})

target_link_libraries(crypto PRIVATE Qt5::Core  "C:/OpenSSL-Win64/libcrypto-3-x64.dll")
