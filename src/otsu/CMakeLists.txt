# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Otsu)

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif (UNIX AND NOT APPLE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

FIND_PACKAGE(OpenCV REQUIRED)

file(
   GLOB_RECURSE
   V_SOURCE
   *
)

add_library(
	Otsu
	${V_SOURCE}
)

#add_library(
#	Otsu
#    STATIC
#	${V_SOURCE}
#)

target_link_libraries(
	Otsu
    ${OpenCV_LIBS}
)
