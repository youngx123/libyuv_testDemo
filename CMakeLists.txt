cmake_minimum_required(VERSION 3.9)
project(yuv_scale)

set(CMAKE_CXX_STANDARD 11)
SET(ROOT_DIR ${CMAKE_SOURCE_DIR})
message("opencv libs : ", ${CMAKE_SOURCE_DIR})

# 添加opencv includes 路径和库
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/opencv45/x86/lib/cmake/opencv4)
set(OpenCV_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/opencv45/x86/include/opencv4)
set(OpenCV_LIBRARIES ${CMAKE_SOURCE_DIR}/opencv45/x86/lib)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/opencv45/x86)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARIES})
find_package(OpenCV REQUIRED)

message("opencv libs : ", ${OpenCV_LIBS})

# libyuv
set(libyuv_DIRS ${CMAKE_SOURCE_DIR}/libyuv/x86)
include_directories(${libyuv_DIRS}/include)

add_library(yuvso STATIC IMPORTED)
set_target_properties(yuvso PROPERTIES IMPORTED_LOCATION ${libyuv_DIRS}/lib/libyuv.a)


# executable
# add_executable(test_yuv_demo yuv2rgb.cpp)
# target_link_libraries(test_yuv_demo yuvso ${OpenCV_LIBS})


# yuy2 to I420
add_executable(yuyv2i420_demo yuv2rgb.cpp)
target_link_libraries(yuyv2i420_demo yuvso ${OpenCV_LIBS})

set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl") 
