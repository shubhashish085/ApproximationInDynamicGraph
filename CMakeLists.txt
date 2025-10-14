cmake_minimum_required(VERSION 2.8)
project(ApproximationInDynamicGraph)

include_directories(
        ${PROJECT_SOURCE_DIR}/
)

set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -O3 -g -Wall -march=native -pthread")

set(APPROXIMATION_FILE_LIST
        CommandParser.h
        CommandParser.cpp
        MatchingCommand.h
        MatchingCommand.cpp
        MascotFD.cpp
        MascotFD.hpp
        ThinkDFD.cpp
        ThinkDFD.hpp
        TriestFD.cpp
        TriestFD.hpp
        graph.cpp
        graph.h
        ProcessGraph.cpp
        types.h)


add_executable(adg.out ${APPROXIMATION_FILE_LIST})