cmake_minimum_required(VERSION 3.2)
project(Mines)
#find_package(Curses REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("./load_ncurses.cmake")

include_directories(${CURSES_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -O2 -std=c++11")

if ($ENV{KMINES_MODE})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_KMINES_MODE")
endif()

set(SOURCES main.cpp game.cpp aho_corasick.cpp)
add_executable(mines ${SOURCES})
target_link_libraries(mines ${CURSES_LIBRARIES})
