cmake_minimum_required(VERSION 3.16.3)
project(strategy)

aux_source_directory(./ SRC)

file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(strategy ${SRC} ${HEADERS})


target_link_libraries(strategy PRIVATE Qt5::Core)
