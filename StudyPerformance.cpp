#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <limits>
#include "graph.h"
#include "types.h"
#include "MatchingCommand.h"
#include "MascotFD.hpp"
#include "ThinkDFD.hpp"
#include "TriestFD.hpp"
#include "PES.hpp"
#include <mpi.h>


void countSquareForThinkDInFullyDynamicGraphStream(const std::string& file_path, const std::string& output_file, ThinkDFD*& module, ui interval,
    ui size, ui rank){
    
    std::ifstream infile(file_path);
    std::ofstream outputfile;
    std::string final_output_file = std::to_string(size) + "_" + std::to_string(rank) + "_" + output_file;
    outputfile.open(final_output_file, std::ios::app);

    if (!infile.is_open()) {
        std::cout << "Can not open the graph file " << file_path << " ." << std::endl;
        exit(-1);
    }

    char type;
    std::string input_line;
    ui label = 0;

    std::cout << "Reading File............ " << std::endl;

    VertexID begin, end;
    std::string addition;

    ui approximated_count = 0, interval_counter = 0, trial_counter = 0;

    double max_error = 0.0, min_error = 30.0;

    while(infile >> begin) {

        infile >> end;
        infile >> addition;

        if(addition == "-"){
            module-> processEdgeSquare(begin, end, false);
        }else{
            module-> processEdgeSquare(begin, end, true);
        }

        interval_counter++;

        if(interval_counter % interval == 0){

            outputfile << interval_counter << "  " << module->getGlobalSquare() << std::endl;
            outputfile.flush();

        }
        
    }


    infile.close();
    outputfile.close();

    //write_into_output_file(output_file, exact_count, global_cnt, error_array, serial);
}


int main(int argc, char** argv){

    MatchingCommand command(argc, argv);

    MPI_Init(NULL, NULL);

    int world_size, world_rank, dest_rank = 0;
    int buffer_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    std::string input_data_graph_file = command.getDataGraphFilePath();
    std::string output_file = command.getOutputFilePath();
    std::string memory_budget_str = command.getMemoryBudget();
    
    
    ui memory_budget = std::stoi(memory_budget_str);
    bool lowerbound = true;
    ui interval = 2000, serial_cnt = 0; 

    std::cout << "Input File : " << input_data_graph_file << std::endl;    
    std::cout << "Memory Budget : " << memory_budget << std::endl;
    std::cout << "Interval : " << interval << std::endl;

    output_file = output_file + "_" + memory_budget_str + ".txt";
 

    ThinkDFD* module = new ThinkDFD(memory_budget, lowerbound);    
    countSquareForThinkDInFullyDynamicGraphStream(input_data_graph_file, output_file, module, interval, world_size, world_rank);    

    MPI_Finalize();    
}