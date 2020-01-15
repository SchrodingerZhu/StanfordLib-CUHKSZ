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
if (WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/libs/windows)
elseif(APPLE)
    link_directories(${CMAKE_SOURCE_DIR}/libs/darwin)
else()
    link_directories(${CMAKE_SOURCE_DIR}/libs/linux)
endif()

add_executable(hello src/hello.h src/hello.cpp)

target_link_libraries(hello stanford-static absl Qt5::Widgets Qt5::Network Qt5::Multimedia Threads::Threads
                                                                                                      ${CMAKE_DL_LIBS})

# Copy resources
file(GLOB resources res/*)
file(COPY ${resources} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
