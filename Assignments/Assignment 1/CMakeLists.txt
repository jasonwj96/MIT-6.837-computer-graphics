cmake_minimum_required(VERSION 3.29)
project(Assignment_1)

set(CMAKE_CXX_STANDARD 17)

file(GLOB VECMATH_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/vecmath/src/*.cpp"
)

file(GLOB PROJECT_SOURCES
        "camera.cpp"
        "curve.cpp"
        "parse.cpp"
        "surf.cpp"
        "main.cpp"
)


find_package(OpenGL REQUIRED)

if (UNIX)
    find_library(GLU_LIBRARY GLU)
endif ()

if (WIN32)
    find_package(GLUT REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
    set(PLATFORM_LIBS ${OPENGL_gl_LIBRARY} ${GLUT_LIBRARIES} gdi32)
else ()
    find_package(GLUT REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
    set(PLATFORM_LIBS ${OPENGL_gl_LIBRARY} ${GLUT_LIBRARIES})
endif ()

add_executable(a1soln
        ${VECMATH_SOURCES}
        ${PROJECT_SOURCES}
)

target_include_directories(a1soln PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/vecmath/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/GL"
)

if (WIN32)
    target_link_libraries(a1soln PRIVATE ${PLATFORM_LIBS}
            opengl32
            glu32
            freeglut
            gdi32)
elseif (UNIX)
    target_link_libraries(a1soln PRIVATE ${PLATFORM_LIBS} OpenGL::GL GLUT::GLUT ${GLU_LIBRARY})
endif ()