# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("audio" C)

add_compile_options(/TC)
add_compile_options(/W3)

# Include sub-projects.
add_subdirectory ("audio")