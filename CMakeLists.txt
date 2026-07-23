cmake_minimum_required(VERSION 3.24)
project(learn-opengl VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/includes")

# ------------------ CREATE EXECUTABLE ------------------ #
add_executable(starting src/starting.cpp)
add_executable(lighting src/lighting.cpp)

# --------------------- IMPORT GLAD --------------------- #
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(starting PRIVATE "${GLAD_DIR}/include")
target_link_libraries(starting "glad" "${CMAKE_DL_LIBS}")
target_include_directories(lighting PRIVATE "${GLAD_DIR}/include")
target_link_libraries(lighting "glad" "${CMAKE_DL_LIBS}")

# --------------------- IMPORT GLFW --------------------- #
set(GLFW_DIR "${LIB_DIR}/glfw")
add_subdirectory("${GLFW_DIR}")
target_include_directories(starting PRIVATE "${GLFW_DIR}/include")
target_link_libraries(starting "glfw" "${GLFW_LIBRARIES}")
target_include_directories(lighting PRIVATE "${GLFW_DIR}/include")
target_link_libraries(lighting "glfw" "${GLFW_LIBRARIES}")

# --------------------- IMPORT GLM --------------------- #
set(GLM_DIR "${LIB_DIR}/glm")
target_include_directories(starting PRIVATE "${GLM_DIR}/glm")
target_include_directories(lighting PRIVATE "${GLM_DIR}/glm")

# --------------------- IMPORT STB --------------------- #
set(STB_DIR "${LIB_DIR}/stb")
target_include_directories(starting PRIVATE "${STB_DIR}")
target_include_directories(lighting PRIVATE "${STB_DIR}")

# -------------------- CHECK PLATFORM -------------------- #
if (APPLE)
    message(STATUS "Platform is Apple")
    add_compile_definitions(PLATFORM_APPLE)
endif ()

if (WIN32)
    message(STATUS "Platform is Windows")
    add_compile_definitions(PLATFORM_WINDOWS)
endif ()