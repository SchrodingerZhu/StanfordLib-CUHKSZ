project(standford)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
find_package(Qt5 COMPONENTS Widgets Network Core Multimedia PrintSupport REQUIRED)
set(CMAKE_CXX_FLAGS "-fPIC ${CMAKE_CXX_FLAGS} -lpthread -ldl -g")
file(GLOB util_files StanfordCPPLib/util/*.h StanfordCPPLib/util/*.cpp)
file(GLOB system_files StanfordCPPLib/system/*.h StanfordCPPLib/system/*.cpp)
file(GLOB graphic_files StanfordCPPLib/graphics/*.h StanfordCPPLib/graphics/*.cpp)
file(GLOB console_files StanfordCPPLib/console/*.h StanfordCPPLib/console/*.cpp)
file(GLOB io_files StanfordCPPLib/io/*.h StanfordCPPLib/io/*.cpp)
file(GLOB collections_files StanfordCPPLib/collections/*.h StanfordCPPLib/collections/*.cpp)
file(GLOB private_files StanfordCPPLib/private/*.h StanfordCPPLib/private/*.cpp)
add_library(stanford SHARED
        StanfordCPPLib/images.qrc
        ${system_files} ${graphic_files} ${collections_files} ${util_files} ${console_files} ${io_files} ${private_files})
add_compile_definitions(${Qt5Widgets_COMPILE_DEFINITIONS})
include_directories(
        StanfordCPPLib
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Multimedia_INCLUDE_DIRS}
)

target_link_libraries(stanford Qt5::Widgets Qt5::Network Qt5::Multimedia)
add_executable(hello src/hello.h src/hello.cpp)
target_link_libraries(hello stanford)