cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

project (Harmonograph)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

find_package(GLFW3 REQUIRED)
if (GLFW3_FOUND)
    message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
    include_directories(${GLFW3_INCLUDE_DIR})
    SET(LIBS ${LIBS} ${GLFW3_LIBRARY})
else()
    message(FATAL_ERROR "GLFW3 not found")
endif()
SET(LIBS ${GLFW3_LIBRARY})

#source files
file( GLOB SRCFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_subdirectory(imgui)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(Harmonograph src/main.cpp)
target_link_libraries(Harmonograph ${LIBS})
target_link_libraries(Harmonograph ${GLFW3_LIBRARY})
target_link_libraries(Harmonograph imgui)