# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Otsu)

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif (UNIX AND NOT APPLE)

if (UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif (UNIX)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIR})
LINK_DIRECTORIES(${OpenCV_LIB_DIR_DBG})


# On Visual 2005 and above, this module can set the debug working directory

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

file(
   GLOB_RECURSE
   V_SOURCE
   *
)


add_executable(
	Otsu
	${V_SOURCE}
)

target_link_libraries(
	Otsu
    SHARED
    ${OpenCV_LIBS}
)