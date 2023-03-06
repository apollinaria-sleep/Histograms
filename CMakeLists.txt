cmake_minimum_required(VERSION 3.21)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
project(__)

set(CMAKE_CXX_STANDARD 17)

find_package(TIFF REQUIRED) # FOO_INCLUDE_DIR, FOO_LIBRARIES
find_package(OpenCV REQUIRED)
include_directories("${TIFF_INCLUDE_DIR}" "${OpenCV_INCLUDE_DIRS}")

add_library(histograms histograms.h histograms.cpp histogram_tests.cpp image.cpp)
add_executable(histogram_tests histogram_tests.cpp)
target_link_libraries(histogram_tests histograms ${TIFF_LIBRARIES} ${OpenCV_LIBS})
