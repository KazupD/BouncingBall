cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0072 NEW)

project(BouncingBall)
enable_testing()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(bouncing_ball src/main.cpp src/ballmanager.cpp src/ball.cpp)

target_link_libraries(bouncing_ball OpenGL::GL GLEW::GLEW glfw)

add_executable(bouncing_unit_test_exe test/unit_test.cpp src/ball.cpp)

target_link_libraries(bouncing_unit_test_exe gtest gtest_main OpenGL::GL)

add_test(NAME bouncing_unit_test COMMAND $<TARGET_FILE:bouncing_unit_test_exe>)

