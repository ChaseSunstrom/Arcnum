cmake_minimum_required(VERSION 3.16)
project(
  Arcnum
  VERSION 0.1.0
  LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Options
option(BUILD_MODS "Build mod plugins" ON)

# Find dependencies using standard modules
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# For GLFW, try standard find_package first, then fallback to pkg-config
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
  find_package(PkgConfig QUIET)
  if(PkgConfig_FOUND)
    pkg_check_modules(GLFW REQUIRED glfw3)
  else()
    message(FATAL_ERROR "GLFW3 not found")
  endif()
endif()

# fmt is optional but recommended
find_package(fmt QUIET)
if(NOT fmt_FOUND)
  message(STATUS "fmt library not found, using standard library formatting")
endif()

# Include subdirectories
add_subdirectory(Spark)
add_subdirectory(Arcnum)

# Optionally build mods
if(BUILD_MODS)

endif()

# Install rules for copying DLLs to bin directory (Windows-specific)
if(WIN32)
  install(TARGETS Spark RUNTIME DESTINATION bin)
endif()
