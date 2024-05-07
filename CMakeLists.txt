cmake_minimum_required(VERSION 3.16)
project(ComputerGraph)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# this two line don't work new.
# set(INCLUDE_FILE)
# set(SRC_FILES)
set(OpenGL_GL_PREFERENCE LEGACY)
set(FREEGLUT_BUILD_DEMOS OFF)
add_subdirectory(thirdparty/freeglut)

# you need pre-install OpenGL, GLU, please google

add_executable(${PROJECT_NAME}
    ${SRC_DIR}/loader.hpp
    ${SRC_DIR}/global.h

    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/loader.cpp
    ${SRC_DIR}/global.c
    ${CMAKE_SOURCE_DIR}/thirdparty/tinyobj_loader_c/tinyobj_loader_c.c
)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${OPENGL_glu_LIBRARY})
target_link_libraries(${PROJECT_NAME} freeglut_static)
target_compile_definitions(${PROJECT_NAME} PRIVATE GL_SILENCE_DEPRECATION)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/freeglut/include/GL
    ${CMAKE_SOURCE_DIR}/thirdparty/tinyobj_loader_c/
    ${INCLUDE_DIR}
)
