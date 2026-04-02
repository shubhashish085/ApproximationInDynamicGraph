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
        DistributedRun.hpp
        DistributedRun.cpp
        DistributedSampling.hpp
        DistributedSampling.cpp
        DistributionCoordinator.hpp
        DistributionCoordinator.cpp
        graph.cpp
        graph.h
        MatchingCommand.h
        MatchingCommand.cpp
        MascotFD.cpp
        MascotFD.hpp
        PES.hpp
        PES.cpp
        PESComputingNode.hpp
        PESComputingNode.cpp
        ProcessGraph.cpp
        ProcessDynamicGraph.cpp
        StudyPerformance.cpp
        ThinkDFD.cpp
        ThinkDFD.hpp
        TriestFD.cpp
        TriestFD.hpp       
        types.h)


add_executable(adg.out ${APPROXIMATION_FILE_LIST})

find_package(MPI REQUIRED)
target_link_libraries(adg.out PUBLIC MPI::MPI_CXX)