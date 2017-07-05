cmake_minimum_required(VERSION 3.7)
project(OpenGLExp)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(OpenGLExp ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(OpenGLExp ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
