cmake_minimum_required(VERSION 3.0)

project(wega)

set(wega_VERSION_MAJOR 0)
set(wega_VERSION_MINOR 1)

add_executable(wega
    src/main.cpp
    src/shader.cpp
    src/chunk.cpp
	src/noiseutils.cpp
)

target_include_directories(wega PUBLIC "./src/")

#GLFW
add_subdirectory("lib/glfw")
target_link_libraries(wega glfw "${GLFW_LIBRARIES}")
target_include_directories(wega PUBLIC "lib/glfw/include")
target_compile_definitions(wega PRIVATE "GLFW_INCLUDE_NONE")

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Builds the GLDW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Builds the GLFW test program")
set(GLFW_BUILD_DOC OFF CACHE INTERNAL "Buils the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generates installation target")

# GLAD
add_library(glad "lib/glad/src/glad.c")
target_include_directories(glad PUBLIC "lib/glad/include")
target_include_directories(wega PRIVATE "lib/glad/include")
target_link_libraries(wega glad "${CMAKE_DL_LIBS}")

# GLM
target_include_directories(wega PRIVATE "lib/glm")

# LIBNOISE
add_subdirectory("lib/noise")
target_link_libraries(wega libnoise)
target_include_directories(wega PUBLIC "lib/noise/include")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# disable all warnings /Wall to enable on msvc -Wall on clang
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /DEBUG /W0 /MTd -std=c++17")

set_property(TARGET wega PROPERTY CXX_STANDARD 17)
set_property(TARGET wega PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "bin/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/wega")

