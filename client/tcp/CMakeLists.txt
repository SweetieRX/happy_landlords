cmake_minimum_required(VERSION 3.16.3)
project(tcp)

# find_package(Qt5 REQUIRED COMPONENTS Network)  # Qt 5

aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(tcp ${SRC} ${HEADERS})

target_link_libraries(tcp PRIVATE Qt5::Core Ws2_32)
