cmake_minimum_required(VERSION 3.16)
project(cg-course)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(thirdparty/freeglut)
find_package(OpenGL REQUIRED)


# you need pre-install OpenGL, GLU, please google
set(INCLUDE_FILES
    ${INCLUDE_DIR}/obj.hpp
    ${INCLUDE_DIR}/stb_image.h
)
set(SRC_FILES
    ${SRC_DIR}/main.cpp
    thirdparty/gltools/src/math3d.cpp
)

add_executable(${PROJECT_NAME}
    ${INCLUDE_FILES}
    ${SRC_FILES}
)
# Add the GLTools source files
add_library(GLTools STATIC thirdparty/gltools/src/GLTools.cpp)
# Specify the directory containing GLTools headers
target_include_directories(GLTools PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/gltools/include
)
# Link GLTools to your executable
target_link_libraries(${PROJECT_NAME} GLTools)

target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
target_link_libraries(${PROJECT_NAME} freeglut_static)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/freeglut/include/GL
    ${CMAKE_SOURCE_DIR}/thirdparty/gltools/include
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)