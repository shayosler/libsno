# Simple CMakeLists.txt file for basic library. Builds library from .cpp files
# in ./src and headers in ./include
cmake_minimum_required(VERSION 3.8)

#################################
## Define the project
set(project_name sno)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "My Library")

#################################
## Library specific includes
#find_package (Python3 COMPONENTS Interpreter Development)
find_package(PythonLibs)
set(libs ${libs} ${PYTHON_LIBRARIES})

#Directory for the rest of our CMake stuff
set(cmake_dir /usr/local/share/cmake/so_cmake)
include(${cmake_dir}/pimpl_lib.cmake)
target_include_directories(${project_name} SYSTEM PUBLIC ${PYTHON_INCLUDE_DIRS})


