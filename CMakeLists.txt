
cmake_minimum_required(VERSION 3.7.2)

project(test VERSION 1.0 LANGUAGES CXX)

# compilation c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#g++ main.cpp -std=c++11 -I/usr/include/python2.7 -lpython2.7 -o essai
add_executable(essai clustering.cpp)
find_package(PythonLibs 2.7)
target_include_directories(essai PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(essai ${PYTHON_LIBRARIES})

message(STATUS " PYTHON_INCLUDE_DIRS=${PYTHON_INCLUDE_DIRS}")
message(STATUS " PYTHON_LIBRARIES=${PYTHON_LIBRARIES}")
