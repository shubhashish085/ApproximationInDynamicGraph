#ifndef ADG_DIST_RUN
#define ADG_DIST_RUN

#include <unordered_map>
#include <unordered_set>
#include "graph.h"
#include "types.h"
#include <dirent.h>
#include <sys/stat.h>
#include "DistributionCoordinator.hpp"


double run_mpi(const char* filename, DistributionCoordinator &coordinator, int workerNum, int memSize, 
    int lenBuf, double tolerance, unsigned int seed, std::vector<float> & oLocalCnt, double &srcCompCost, double &workerCompCostMax, double &workerCompCostSum);

double run_mpi_pes(const char* filename, DistributionCoordinator &hIO, int workerNum, int memSize, ui pool_size, double sample_probability,
    int lenBuf, double tolerance, unsigned int seed, std::vector<float> & oLocalCnt, double &srcCompCost, double &workerCompCostMax, double &workerCompCostSum);

void run_exp (const char* input, const char* outPath, DistributionCoordinator &coordinator, int workerNum, 
    int memSize, int repeat, int bufLen=1000, double tolerance=0.2);

void run_exp_pes (const char* input, const char* outPath, DistributionCoordinator &coordinator, int workerNum, 
    int memSize, int repeat, int bufLen=1000, double tolerance=0.2);

#endif