cmake_minimum_required(VERSION 3.24)
project(learn-opengl VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/includes")

# ------------------ CREATE EXECUTABLE ------------------ #
add_executable(starting ${SRC_DIR}/starting.cpp)
add_executable(lighting ${SRC_DIR}/lighting.cpp)
add_executable(model_loading ${SRC_DIR}/model_loading.cpp)
add_executable(depth_testing ${SRC_DIR}/depth_testing.cpp)

# --------------------- IMPORT GLAD --------------------- #
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
include_directories("${GLAD_DIR}/include")
target_link_libraries(starting "glad" "${CMAKE_DL_LIBS}")
target_link_libraries(lighting "glad" "${CMAKE_DL_LIBS}")

# # --------------------- IMPORT GLFW --------------------- #
set(GLFW_DIR "${LIB_DIR}/glfw")
add_subdirectory("${GLFW_DIR}")
include_directories("${GLFW_DIR}/include")
target_link_libraries(starting "glfw" "${GLFW_LIBRARIES}")
target_link_libraries(lighting "glfw" "${GLFW_LIBRARIES}")

# # --------------------- IMPORT GLM --------------------- #
set(GLM_DIR "${LIB_DIR}/glm")
include_directories("${GLM_DIR}/glm")

# # --------------------- IMPORT STB --------------------- #
set(STB_DIR "${LIB_DIR}/stb")
include_directories("${STB_DIR}")

# # --------------------- IMPORT ASSIMP ------------------ #
set(ASSIMP_DIR "${LIB_DIR}/assimp")
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_MINIZIP ON)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
add_subdirectory("${ASSIMP_DIR}")
target_include_directories(model_loading PRIVATE "${ASSIMP_DIR}/include")
target_link_libraries(model_loading PRIVATE assimp glad glfw)
target_include_directories(depth_testing PRIVATE "${ASSIMP_DIR}/include")
target_link_libraries(depth_testing PRIVATE assimp glad glfw)

# -------------------- CHECK PLATFORM -------------------- #
if (APPLE)
    message(STATUS "Platform is Apple")
    add_compile_definitions(PLATFORM_APPLE)
endif ()

if (WIN32)
    message(STATUS "Platform is Windows")
    add_compile_definitions(PLATFORM_WINDOWS)
endif ()