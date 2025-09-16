cmake_minimum_required(VERSION 3.16.3)
project(player)

aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(player ${SRC} ${HEADERS})

target_link_libraries(player PRIVATE Qt5::Core)
