cmake_minimum_required(VERSION 3.16.3)
project(serialize)

include_directories("C:/Protobuf/include")
aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(serialize ${SRC} ${HEADERS} information.proto)

target_link_libraries(serialize PRIVATE Qt5::Core "C:/Protobuf/lib/libprotobufd.dll")
