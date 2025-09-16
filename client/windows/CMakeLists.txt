cmake_minimum_required(VERSION 3.16.3)
project(windows)

aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB UIS ${PROJECT_SOURCE_DIR}/*.ui)

add_library(windows ${SRC} ${HEADERS} ${UIS})

target_link_libraries(windows PRIVATE Qt5::Core Qt5::Widgets Qt5::Multimedia)
