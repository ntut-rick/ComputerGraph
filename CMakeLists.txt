cmake_minimum_required(VERSION 3.16)
project(ComputerGraph)

include(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake)
FetchContent_MakeAvailable(freeglut_static)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(THIRDPARTY ${CMAKE_SOURCE_DIR}/thirdparty)

set(INCLUDE_FILES
    ${THIRDPARTY}/m3d/include/math3d.h
)
set(SRC_FILES
    ${THIRDPARTY}/m3d/src/math3d.cpp
)

# you need pre-install OpenGL, GLU, please google
add_executable(${PROJECT_NAME}
    ${SRC_DIR}/main.cpp ${INCLUDE_FILES} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${OPENGL_glu_LIBRARY})
target_link_libraries(${PROJECT_NAME} freeglut_static)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/freeglut/include/GL
    ${THIRDPARTY}/m3d/include
)
