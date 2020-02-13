file(GLOB_RECURSE "${PROJECT_NAME}_SOURCES" RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
src/*.cpp
src/*.h
)
MESSAGE(STATUS "${PROJECT_NAME}_SOURCES: ${${PROJECT_NAME}_SOURCES}")

if (MSVC)
  include_directories(${WX_DIR}/include ${WX_DIR}/include/msvc)
  if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8") # Checks if system is 64 bits
    link_directories(${WX_DIR}/lib/vc_x64_lib)
  else()
    link_directories(${WX_DIR}/lib/vc_lib)
  endif("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")  
endif (MSVC)
if(WIN32)
  add_compile_definitions(__WXMSW__)
  add_compile_definitions(_UNICODE)
  add_executable(${PROJECT_NAME} WIN32 ${${PROJECT_NAME}_SOURCES})
endif(WIN32)
if(UNIX)
  add_compile_definitions(__WXGTK__)
  add_compile_definitions(_UNICODE)
  add_subdirectory(vendor/wxWidgets)
  add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
  target_link_libraries(${PROJECT_NAME} PUBLIC gl net core base ${X11_LIBRARIES})
endif(UNIX)

target_precompile_headers(${PROJECT_NAME} PUBLIC src/ftpch.h)

target_link_libraries (${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
target_link_libraries (${PROJECT_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})