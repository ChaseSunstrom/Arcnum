set(SPARK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SPARK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Collect source files
file(GLOB_RECURSE SPARK_SOURCES "${SPARK_SOURCE_DIR}/*.cpp")

file(GLOB_RECURSE SPARK_HEADERS "${SPARK_INCLUDE_DIR}/*.hpp")

# Create shared library
add_library(Spark SHARED ${SPARK_SOURCES} ${SPARK_HEADERS})

# Set includes
target_include_directories(
  Spark
  PUBLIC $<BUILD_INTERFACE:${SPARK_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>
  PRIVATE ${SPARK_SOURCE_DIR})

# Link dependencies
target_link_libraries(
  Spark
  PUBLIC OpenGL::GL
  PRIVATE GLEW::GLEW glfw fmt::fmt)

# Define preprocessor macros
target_compile_definitions(
  Spark
  PRIVATE SPARK_EXPORTS
  PUBLIC $<$<BOOL:${WIN32}>:NOMINMAX WIN32_LEAN_AND_MEAN>)

# Platform-specific configuration
if(MSVC)
  target_compile_options(Spark PRIVATE /W4)
else()
  target_compile_options(Spark PRIVATE -Wall -Wextra)
endif()

# Setup precompiled headers
target_precompile_headers(Spark PRIVATE ${SPARK_INCLUDE_DIR}/spark_pch.hpp)

# Export Spark target for use by other subprojects
set(SPARK_LIBRARY
    Spark
    CACHE INTERNAL "")

# Install
install(
  TARGETS Spark
  EXPORT SparkTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES
  DESTINATION include)

install(
  DIRECTORY ${SPARK_INCLUDE_DIR}/
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.hpp")

install(
  EXPORT SparkTargets
  FILE SparkTargets.cmake
  NAMESPACE Spark::
  DESTINATION lib/cmake/Spark)
