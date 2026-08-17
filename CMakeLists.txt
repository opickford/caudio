# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("soundforc" VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOUNDFORC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SOUNDFORC_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

configure_file(
	${SOUNDFORC_SOURCE_DIR}/cmake/version.h.in
	${SOUNDFORC_BINARY_DIR}/include/sfc/version.h
)

add_compile_options(/TC)
add_compile_options(/W3)

# TODO: Set output directories for targets?

add_subdirectory(src)


option(SOUNDFORC_BUILD_EXAMPLES "Build examples" ON)
option(SOUNDFORC_BUILD_TESTS "Build tests" ON)
option(SOUNDFORC_INSTALL "Enable installation" ON)

if (SOUNDFORC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (SOUNDFORC_BUILD_TESTS)
    # add_subdirectory(tests)
endif()

if (SOUNDFORC_INSTALL)
    include(cmake/install.cmake)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "example")