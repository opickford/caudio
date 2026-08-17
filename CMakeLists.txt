# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("caudio" VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CAUDIO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CAUDIO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

configure_file(
	${CAUDIO_SOURCE_DIR}/cmake/version.h.in
	${CAUDIO_BINARY_DIR}/include/caudio/version.h
)

add_compile_options(/TC)
add_compile_options(/W3)


# TODO: Set output directories for targets?

add_subdirectory(src)


option(CAUDIO_BUILD_EXAMPLES "Build examples" ON)
option(CAUDIO_BUILD_TESTS "Build tests" ON)
option(CAUDIO_INSTALL "Enable installation" ON)


if (CAUDIO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (CAUDIO_BUILD_TESTS)
    # add_subdirectory(tests)
endif()

if (CAUDIO_INSTALL)
    include(cmake/install.cmake)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "example")