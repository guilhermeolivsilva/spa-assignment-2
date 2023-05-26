cmake_minimum_required(VERSION 3.16)
project(DEAD_CODE_ELIMINATION)

find_package(LLVM REQUIRED CONFIG)
execute_process(COMMAND llvm-config --cxxflags --ldflags OUTPUT_VARIABLE LLVM_CONFIG_N)
include_directories(${LLVM_INCLUDE_DIRS})

string(REPLACE "\n" " " LLVM_CONFIG "${LLVM_CONFIG_N}")
message(STATUS "LLVM_CONFIG: ${LLVM_CONFIG}")

set(CMAKE_CXX_FLAGS "-O3 -g ${LLVM_CONFIG}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fdiagnostics-color=always -fno-exceptions -fno-rtti -std=c++11 -D__GLIBCXX_USE_CXX11_ABI=0")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

include_directories(RangeAnalysis)

add_library (DeadCodeElimination SHARED
RangeAnalysis/RangeAnalysis.cpp
RangeAnalysis/vSSA.cpp
RangeAnalysis/DeadCodeElimination.cpp
)
