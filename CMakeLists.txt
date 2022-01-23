cmake_minimum_required(VERSION 2.6)
project(monitor)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
file(GLOB TEST_SOURCES "src/[^m]*.cpp") # ugly solution to exclude the main
file(GLOB TEST_SUITE "test/testSuite.cpp")

add_executable(monitor ${SOURCES})
add_executable(test ${TEST_SOURCES} ${TEST_SUITE})

set_property(TARGET monitor PROPERTY CXX_STANDARD 17)
target_link_libraries(monitor ${CURSES_LIBRARIES})
# TODO: Run -Werror in CI.
target_compile_options(monitor PRIVATE -Wall -Wextra)

set_property(TARGET test PROPERTY CXX_STANDARD 17)
target_link_libraries(test ${CURSES_LIBRARIES})
# TODO: Run -Werror in CI.
target_compile_options(test PRIVATE -Wall -Wextra)
