cmake_minimum_required(VERSION 3.5)
project(OpenGLExp)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c jitter.h accanti.c triangle.c all.h grid.c triangle.cpp gol.c exp.c teapot.c cyril.c xbox.c together.c dualwindow.c dualwindow2.c)
add_executable(OpenGLExp ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package (Threads REQUIRED)
find_package(libfreenect REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${FREENECT_INCLUDE_DIRS} ${THREADS_PTHREADS_INCLUDE_DIR})

target_link_libraries(OpenGLExp ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${SDL2_LIBRARY} ${GLEW_LIBRARIES} ${FREENECT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
