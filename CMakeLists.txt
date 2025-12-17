# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("audio" C)

add_compile_options(/TC)
add_compile_options(/W3)


# TODO: Set output directories for targets?

add_subdirectory(src)


option(AUDIO_BUILD_EXAMPLES "Build examples" ON)
option(AUDIO_BUILD_TESTS "Build tests" ON)
option(AUDIO_INSTALL "Enable installation" ON)


if (AUDIO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (AUDIO_BUILD_TESTS)
    # add_subdirectory(tests)
endif()

if (AUDIO_INSTALL)
    # include(cmake/install.cmake)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "example")