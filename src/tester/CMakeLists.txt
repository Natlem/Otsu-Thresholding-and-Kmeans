# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Tester)

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif (UNIX AND NOT APPLE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

FIND_PACKAGE(OpenCV REQUIRED)

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/bin)

# On Visual 2005 and above, this module can set the debug working directory

file(
   GLOB_RECURSE
   V_SOURCE
   *
)


add_executable(
	Tester
	${V_SOURCE}
)

if ( WIN32)
target_link_libraries(
	Tester
    ${OpenCV_LIBS}
    Otsu.dll
)
else()
target_link_libraries(
	Tester
    ${OpenCV_LIBS}
    Otsu.a
)
endif()
