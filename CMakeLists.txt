cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

project (Harmonograph)

# Set C++11 flag
list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

# Set module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Find required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL")

# Set libraries
set(LIBS ${GLFW3_LIBRARY} ${GLM_LIBRARY} ${OPENGL_LIBRARIES})

# Source files
file(GLOB SRCFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

# Create source library
add_library(source_lib STATIC ${SRCFILES})

# Create GLAD library
add_library(GLAD "src/glad.c")

# Add GLAD and includes directory to include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/glad)
include_directories(${CMAKE_SOURCE_DIR}/include/GL)

# Create executable
add_executable(Harmonograph src/main.cpp)

# Link libraries
target_link_libraries(Harmonograph source_lib ${LIBS})
