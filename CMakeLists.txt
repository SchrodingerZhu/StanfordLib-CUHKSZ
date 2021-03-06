# Project Name
project(stanford)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(abseil-cpp)

# Cmake Configuration

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# Find Qt5 Packages
find_package(Qt5 COMPONENTS Widgets Network Core Multimedia PrintSupport REQUIRED)
find_package(Threads REQUIRED)
# Define Library Files
file(GLOB util_files StanfordCPPLib/util/*.h StanfordCPPLib/util/*.cpp)
file(GLOB system_files StanfordCPPLib/system/*.h StanfordCPPLib/system/*.cpp)
file(GLOB graphic_files StanfordCPPLib/graphics/*.h StanfordCPPLib/graphics/*.cpp)
file(GLOB console_files StanfordCPPLib/console/*.h StanfordCPPLib/console/*.cpp)
file(GLOB io_files StanfordCPPLib/io/*.h StanfordCPPLib/io/*.cpp)
file(GLOB collections_files StanfordCPPLib/collections/*.h StanfordCPPLib/collections/*.cpp)
file(GLOB private_files StanfordCPPLib/private/*.h StanfordCPPLib/private/*.cpp)
file(GLOB sync_files StanfordCPPLib/sync/*.h StanfordCPPLib/sync/*.cpp)

# Define Stanford Library
add_library(stanford SHARED
        StanfordCPPLib/images.qrc
        ${system_files} ${graphic_files} ${collections_files} ${util_files} ${console_files} ${io_files} ${private_files} ${sync_files})
add_library(stanford-static STATIC
        StanfordCPPLib/images.qrc
        ${system_files} ${graphic_files} ${collections_files} ${util_files} ${console_files} ${io_files} ${private_files} ${sync_files})

# Include Dirs
include_directories(
        StanfordCPPLib
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Multimedia_INCLUDE_DIRS}
)

# Linking Relations
target_link_libraries(stanford PUBLIC Qt5::Widgets
        Qt5::Network
        Qt5::Multimedia
        absl::container
        absl::strings
        absl::utility
        absl::str_format
        absl::hashtablez_sampler
        absl::raw_hash_set
        absl::flat_hash_map
        Threads::Threads
        ${CMAKE_DL_LIBS}
        )
target_link_libraries(stanford-static PUBLIC Qt5::Widgets
        Qt5::Network
        Qt5::Multimedia
        absl::container
        absl::strings
        absl::utility
        absl::str_format
        absl::hashtablez_sampler
        absl::raw_hash_set
        absl::flat_hash_map
        Threads::Threads
        ${CMAKE_DL_LIBS}
        )

add_executable(hello src/hello.h src/hello.cpp)
target_link_libraries(hello stanford)

set_target_properties(stanford PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set_target_properties(stanford-static PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
target_link_libraries(hello stanford)



# Copy resources
file(GLOB resources res/*)
file(COPY ${resources} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#Unit tests
include(tests/tests.cmake)
