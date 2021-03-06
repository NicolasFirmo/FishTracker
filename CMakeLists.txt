cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

project(FishTracker VERSION 0.1.0)

if (UNIX)
  find_package (X11 REQUIRED)
  message(STATUS "X11_FOUND = ${X11_FOUND}")
  message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
  message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")
endif (UNIX)
find_package (Threads)


set(WX_DIR $ENV{WXWIN})
set(OpenCV_DIR $ENV{OPENCV_DIR})

# OpenCV
set(OpenCV_STATIC ON)

MESSAGE(STATUS "OpenCV_DIR ${OpenCV_DIR}")

find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR})

MESSAGE(STATUS "OpenCV Include dirs ${OpenCV_INCLUDE_DIRS}")
MESSAGE(STATUS "OpenCV LINK LIBRARIES ${OpenCV_LIBS}")

include_directories( ${OpenCV_INCLUDE_DIRS} )
if (UNIX)
  include_directories(${X11_INCLUDE_DIR})
endif (UNIX)

# Project subdirectory
add_subdirectory(${PROJECT_NAME})

# Preprocessor
if (UNIX)
  target_compile_definitions(${PROJECT_NAME} PUBLIC "FT_PLATFORM_LINUX")
endif (UNIX)
if (WIN32)
  target_compile_definitions(${PROJECT_NAME} PUBLIC "FT_PLATFORM_WINDOWS")
endif (WIN32)
if (MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif (MSVC)

target_compile_definitions(${PROJECT_NAME} PUBLIC
 "$<$<CONFIG:DEBUG>:FT_ENABLE_ASSERTS>"
 )