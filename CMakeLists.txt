cmake_minimum_required(VERSION 3.16)
project(ComputerGraph)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
add_subdirectory(thirdparty/freeglut)

# this two line don't work new.
set(INCLUDE_FILES
    ${INCLUDE_DIR}/NiuBiObject.hpp
    ${INCLUDE_DIR}/stb_image.h
    ${INCLUDE_DIR}/myglTransform.h
)
set(SRC_FILES
)

add_executable(${PROJECT_NAME}
    ${SRC_DIR}/main.cpp
    ${INCLUDE_FILES}
    ${SRC_FILES}
)

target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
target_link_libraries(${PROJECT_NAME} freeglut_static)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/freeglut/include/GL)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
