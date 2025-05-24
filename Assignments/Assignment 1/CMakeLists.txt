cmake_minimum_required(VERSION 3.29)
project(Assignment_1)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(cmake-build-debug)
include_directories(GL)
include_directories(lib)
include_directories(swp)
include_directories(vecmath)
include_directories(vecmath/include)
include_directories(vecmath/src)

add_executable(a1soln
        GL/freeglut.h
        GL/freeglut_ext.h
        GL/freeglut_std.h
        lib/freeglut.lib
        swp/circles.swp
        swp/core.swp
        swp/flircle.swp
        swp/florus.swp
        swp/gentorus.swp
        swp/norm.swp
        swp/tor.swp
        swp/weird.swp
        swp/weirder.swp
        swp/wineglass.swp
        vecmath/include/Matrix2f.h
        vecmath/include/Matrix3f.h
        vecmath/include/Matrix4f.h
        vecmath/include/Quat4f.h
        vecmath/include/vecmath.h
        vecmath/include/Vector2f.h
        vecmath/include/Vector3f.h
        vecmath/include/Vector4f.h
        vecmath/src/Matrix2f.cpp
        vecmath/src/Matrix3f.cpp
        vecmath/src/Matrix4f.cpp
        vecmath/src/Quat4f.cpp
        vecmath/src/Vector2f.cpp
        vecmath/src/Vector3f.cpp
        vecmath/src/Vector4f.cpp
        camera.cpp
        camera.h
        curve.cpp
        curve.h
        extra.h
        parse.cpp
        parse.h
        surf.cpp
        surf.h
        tuple.h
        main.cpp)

target_link_libraries(a1soln opengl32 glu32 gdi32 freeglut)