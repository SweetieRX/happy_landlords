cmake_minimum_required(VERSION 3.16.3)
project(control)

include_directories(
    "C:/Protobuf/"
)
aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(control ${SRC} ${HEADERS})

target_link_libraries(control PRIVATE Qt5::Core Qt5::Multimedia)
