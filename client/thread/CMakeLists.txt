cmake_minimum_required(VERSION 3.16.3)
project(thread)

aux_source_directory(./ SRC)

file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

find_package(Qt5 REQUIRED COMPONENTS Core)

add_library(thread ${SRC} ${HEADERS})


target_link_libraries(thread PRIVATE Qt5::Core)
target_link_libraries(thread PRIVATE Qt5::Core)
