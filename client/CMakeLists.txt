cmake_minimum_required(VERSION 3.16.3)

project(client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED Multimedia)
qt5_add_resources(RES res.qrc)

include_directories(
    ${PROJECT_SOURCE_DIR}/card
    ${PROJECT_SOURCE_DIR}/windows
    ${PROJECT_SOURCE_DIR}/strategy
    ${PROJECT_SOURCE_DIR}/control
    ${PROJECT_SOURCE_DIR}/player
    ${PROJECT_SOURCE_DIR}/thread
    ${PROJECT_SOURCE_DIR}/tcp
    ${PROJECT_SOURCE_DIR}/serialize
    ${PROJECT_SOURCE_DIR}/crypto
    "C:/OpenSSL-Win64/include"
    "C:/Protobuf/include"

)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set(ICON_RES ${CMAKE_CURRENT_SOURCE_DIR}/icon.rc)
endif()

link_directories("C:\\Protobuf\\lib")

add_subdirectory(card)
add_subdirectory(windows)
add_subdirectory(strategy)
add_subdirectory(control)
add_subdirectory(player)
add_subdirectory(thread)
add_subdirectory(tcp)
add_subdirectory(serialize)
add_subdirectory(crypto)




if(ANDROID)
  add_library(client SHARED
    main.cpp
  )
else()
  add_executable(client
    WIN32
    main.cpp
    ${RES}
    ${ICON_RES}
  )
endif()

target_link_libraries(client PRIVATE Qt5::Widgets card windows control player strategy thread tcp serialize crypto)




