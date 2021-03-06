# Project Name
project(my_project)

# Cmake Configuration
cmake_minimum_required(VERSION 3.12)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 14)
# Find Packages
find_package(Qt5 COMPONENTS Widgets Network Core Multimedia PrintSupport REQUIRED)
find_package(Threads REQUIRED)

# Include Dirs
include_directories(
        includes/abseil
        includes/mimalloc
        includes/stanford
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Multimedia_INCLUDE_DIRS}
)

# Linking Relations
link_directories(${CMAKE_SOURCE_DIR}/libs)
add_executable(hello src/hello.h src/hello.cpp)

target_link_libraries(hello stanford Qt5::Widgets Qt5::Network Qt5::Multimedia Threads::Threads
                                                                                                      ${CMAKE_DL_LIBS})

# Copy resources
file(GLOB resources res/*)
file(COPY ${resources} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
if (WIN32)
    file(COPY libs/libstanford.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()