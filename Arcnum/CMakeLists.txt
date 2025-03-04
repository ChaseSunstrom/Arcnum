set(ARCNUM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Collect source files
file(GLOB_RECURSE ARCNUM_SOURCES "${ARCNUM_SOURCE_DIR}/*.cpp")

file(GLOB_RECURSE ARCNUM_HEADERS "${ARCNUM_SOURCE_DIR}/*.hpp")

# Create executable
add_executable(Arcnum ${ARCNUM_SOURCES} ${ARCNUM_HEADERS})

# Set includes
target_include_directories(Arcnum PRIVATE ${ARCNUM_SOURCE_DIR}
                                          ${SPARK_INCLUDE_DIR})

# Link with Spark
target_link_libraries(Arcnum PRIVATE ${SPARK_LIBRARY})

# Platform-specific settings
if(MSVC)
  target_compile_options(Arcnum PRIVATE /W4)
else()
  target_compile_options(Arcnum PRIVATE -Wall -Wextra)
endif()

# Setup post-build commands to copy Spark DLL to binary directory
if(WIN32)
  add_custom_command(
    TARGET Arcnum
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Spark>
            $<TARGET_FILE_DIR:Arcnum>)
endif()

# Install
install(TARGETS Arcnum RUNTIME DESTINATION bin)
