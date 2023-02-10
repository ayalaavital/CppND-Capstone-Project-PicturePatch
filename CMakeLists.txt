cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(PicturePatch)

find_package(OpenCV 4.1 REQUIRED)

include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})

link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

find_package(PkgConfig)
pkg_check_modules(YAMLCPP REQUIRED yaml-cpp>=0.5)
include_directories(${YAMLCPP_INCLUDE_DIRS})

add_executable(PicturePatch src/main.cpp src/image.cpp src/parser.cpp src/app.cpp)

target_link_libraries(PicturePatch ${OpenCV_LIBS} ${catkin_LIBRARIES} ${YAMLCPP_LIBRARIES})


# https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/

# include_directories(${gtest_SOURCE_DIR})
# add_executable(test test/utest_image.cpp src/image.cpp)
# target_link_libraries(test gtest gtest_main pthread ${OpenCV_LIBS})