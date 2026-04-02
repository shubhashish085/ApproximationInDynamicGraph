#ifndef ADG_DIST_RUN
#define ADG_DIST_RUN

#include <unordered_map>
#include <unordered_set>
#include "graph.h"
#include "types.h"
#include <dirent.h>
#include <sys/stat.h>
#include "DistributionCoordinator.hpp"


double run_mpi(const char* filename, std::string output_file_name, DistributionCoordinator &coordinator, int workerNum, int memSize, 
    int lenBuf, double tolerance, unsigned int seed, std::vector<float> & oLocalCnt, double &srcCompCost, double &workerCompCostMax, double &workerCompCostSum);

double run_mpi_pes(const char* filename, std::string output_file_name, DistributionCoordinator &hIO, int workerNum, int memSize, ui pool_size, double sample_probability,
    int lenBuf, double tolerance, unsigned int seed, std::vector<float> & oLocalCnt, double &srcCompCost, double &workerCompCostMax, double &workerCompCostSum);

void run_exp (const char* input, std::string outPath, DistributionCoordinator &coordinator, int workerNum, 
    int memSize, int repeat, int bufLen=1000, double tolerance=0.2);

void run_exp_pes (const char* input, std::string outPath, DistributionCoordinator &coordinator, int workerNum, 
    int memSize, int repeat, int bufLen=1000, double tolerance=0.2);

void write_worker_results_to_output_file(const char* input_graph_file, std::string output_file_path, std::string algorithm_name, ui memory_budget, ui interval,
   std::vector<double> global_cnt, std::vector<double> elapsed_time){

    std::ofstream outputfile;
    outputfile.open(output_file_path, std::ios::app);

    std::cout << "Input File : " << input_graph_file << std::endl;
    std::cout << "Sampling Technique  : " << algorithm_name << std::endl;
    std::cout << "Memory Budget : " << memory_budget << std::endl;
    std::cout << "Interval : " << interval << std::endl;

    ui serial = 0;
    outputfile << "Serial" << "  " << "Estimated_Count" << "  " << "Elapsed_Time" << std::endl;


    for (ui i = 0; i < global_cnt.size(); i++){
        outputfile << serial << "  " << global_cnt[i] << "  " << elapsed_time[i] << std::endl;
        serial += interval;

        if(i % 50 == 0){
            outputfile.flush();
        }
    }

    outputfile.flush();
    outputfile.close();
}


#endif