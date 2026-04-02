#include <fstream>
#include "PESComputingNode.hpp"
#include "DistributedSampling.hpp"
#include "DistributedRun.hpp"
#include "DistributionCoordinator.hpp"

double run_mpi(const char* filename, std::string output_file_name, DistributionCoordinator &hIO, int workerNum, int memSize, int lenBuf, double tolerance, unsigned int seed, std::vector<float> & oLocalCnt, double &srcCompCost, double &workerCompCostMax, double &workerCompCostSum)
{
    clock_t begin = clock();

    hIO.init(lenBuf, workerNum);

    // Sourcce init
    if (hIO.isMaster())
    {
        ComputingNode masterNode;
        Edge edge;

        std::ifstream infile(filename);

        if (!infile.is_open()) {
            std::cout << "Can not open the graph file " << filename << " ." << std::endl;
            exit(-1);
        }

        VertexID begin, end;
        std::string addition;


        while (infile >> begin) // Stream edges
        {
            infile >> end;
            infile >> addition;

            edge.src = begin;
            edge.dst = end;

            if(addition == "-"){
                edge.add = false;
            }else{
                edge.add = true;
            }


            if(edge.src != edge.dst) {
                masterNode.processEdgeForMaster(edge);
                hIO.bCastEdge(edge);
            }
        }

        hIO.sendEndSignal();

        double globalCnt = 0;

        hIO.recvCnt(masterNode.maxVertexId, globalCnt, oLocalCnt);

        hIO.recvTime(workerCompCostMax, workerCompCostSum);


        globalCnt = globalCnt / workerNum;
        for(auto it = oLocalCnt.begin(); it != oLocalCnt.end(); ++it)
        {
            *it  = *it / workerNum;
        }


        srcCompCost = (double(clock() - begin) - hIO.getIOCPUTime()) / CLOCKS_PER_SEC; // source cpu time

        return globalCnt;
    }
    else // Worker part
    {
        ComputingNode  worker(memSize, seed + hIO.getWorkerId());
        ui edge_cnt = 0;
        Edge edge;

        double total_time = 0, per_execution_time;

        std::vector<double> global_cnt_vtr;
        std::vector<double> elapsed_time_vtr;

        std::string output_file = "Sq_" + output_file_name + "_p_" + std::to_string(hIO.getRank()) + ".txt";


        while(hIO.recvEdge(edge))
        {
            std::clock_t begin = clock(); 
            worker.processEdgeForWorker(edge);
            per_execution_time = (double)(clock() - begin) / CLOCKS_PER_SEC ;
            total_time += per_execution_time; 

            edge_cnt += 1;

            if(edge_cnt % 1000 == 0){
                global_cnt_vtr.push_back(worker.globalCnt);
                elapsed_time_vtr.push_back(total_time);
                total_time = 0;
            }
        }

        // send counts to master
        hIO.sendCnt(worker.getGlobalCnt(), worker.getLocalCnt());

        double workerCompCost = (double(clock() - begin) - hIO.getIOCPUTime()) / CLOCKS_PER_SEC; // source cpu time

        hIO.sendTime(workerCompCost);

        write_worker_results_to_output_file(filename, output_file, ALG_THINKD_SQR, memSize, INTERVAL, global_cnt_vtr, elapsed_time_vtr);

        return 0;
    }

}


double run_mpi_pes(const char* filename, std::string output_file_name, DistributionCoordinator &hIO, int workerNum, int memSize, ui pool_size, double sample_probability,
 int lenBuf, double tolerance, unsigned int seed, std::vector<float> & oLocalCnt, double &srcCompCost, double &workerCompCostMax, double &workerCompCostSum)
{
    clock_t begin = clock();

    hIO.init(lenBuf, workerNum);

    // Sourcce init
    if (hIO.isMaster())
    {
        ComputingNode masterNode;
        Edge edge;

        std::ifstream infile(filename);

        if (!infile.is_open()) {
            std::cout << "Can not open the graph file " << filename << " ." << std::endl;
            exit(-1);
        }

        VertexID begin, end;
        std::string addition;


        while (infile >> begin) // Stream edges
        {
            infile >> end;
            infile >> addition;

            edge.src = begin;
            edge.dst = end;

            if(addition == "-"){
                edge.add = false;
            }else{
                edge.add = true;
            }


            if(edge.src != edge.dst) {
                masterNode.processEdgeForMaster(edge);
                hIO.bCastEdge(edge);
            }
        }

        hIO.sendEndSignal();
        //std::cout << "Master: " << double(clock() - begin) / CLOCKS_PER_SEC << "\t" << hIO.getIOCPUTime() / CLOCKS_PER_SEC << "\t" <<  srcCompCost << endl;

        // Gather results from curWorkers
        double globalCnt = 0;

        // communication cost for gather
        hIO.recvCnt(masterNode.maxVertexId, globalCnt, oLocalCnt);

        //std::cout << source.getMaxVId() << "\t" << globalCnt << "\t" << oLocalCnt.size();

        hIO.recvTime(workerCompCostMax, workerCompCostSum);


        globalCnt = globalCnt / workerNum;
        for(auto it = oLocalCnt.begin(); it != oLocalCnt.end(); ++it)
        {
            *it  = *it / workerNum;
        }


        srcCompCost = (double(clock() - begin) - hIO.getIOCPUTime()) / CLOCKS_PER_SEC; // source cpu time

        return globalCnt;
    }
    else // Worker part
    {

        //std::cout << "worker begins..." << endl;
        ui memory_budget;
        double sample_probability;
        ui pool_size;

        PESComputingNode  worker(memory_budget, sample_probability, pool_size);
        Edge edge;

        while(hIO.recvEdge(edge)){
            worker.processEdgeForPesInCocos(edge);
        }

        // send counts to master
        hIO.sendCnt(worker.getGlobalCnt(), worker.getLocalCnt());

        double workerCompCost = (double(clock() - begin) - hIO.getIOCPUTime()) / CLOCKS_PER_SEC; // source cpu time

        hIO.sendTime(workerCompCost);
        return 0;
    }
}


void run_exp (const char* input, std::string outPath, DistributionCoordinator &hIO, int workerNum, int memSize, int repeat, int bufLen, double tolerance)
{

    int seed = 0;

    struct timeval diff, startTV, endTV;

	if (hIO.isMaster())
	{
		
	}

    for(int i =0 ; i < repeat; i++) {

        if (hIO.isMaster()) {

            gettimeofday(&startTV, NULL);

            std::vector<float> nodeToCnt;

            double srcCompCost = 0;
            double workerCompCostMax = 0;
            double workerCompCostSum = 0;

            double globalCnt = run_mpi(input, outPath, hIO, workerNum, memSize, bufLen, tolerance, seed + repeat * workerNum * i, nodeToCnt, srcCompCost, workerCompCostMax, workerCompCostSum);

            gettimeofday(&endTV, NULL);

            timersub(&endTV, &startTV, &diff);

            double elapsedTime = diff.tv_sec * 1000 + diff.tv_usec / 1000 ;

        } else {

            double srcCompCost = 0;
            double workerCompCostMax = 0;
            double workerCompCostSum = 0;
            std::vector<float> nodeToCnt;
            run_mpi(input, outPath, hIO, workerNum, memSize, bufLen, tolerance, seed + repeat * workerNum * i, nodeToCnt, srcCompCost, workerCompCostMax, workerCompCostSum);
        }
    }
}


void run_exp_pes (const char* input, std::string outPath, DistributionCoordinator &hIO, int workerNum, int memSize, int repeat, int bufLen, double tolerance)
{

    int seed = 0;

    struct timeval diff, startTV, endTV;

	if (hIO.isMaster())
	{
		
	}

    for(int i =0 ; i < repeat; i++) {

        if (hIO.isMaster()) {

            gettimeofday(&startTV, NULL);

            std::vector<float> nodeToCnt;

            double srcCompCost = 0;
            double workerCompCostMax = 0;
            double workerCompCostSum = 0;

            double globalCnt = run_mpi(input, outPath, hIO, workerNum, memSize, bufLen, tolerance, seed + repeat * workerNum * i, nodeToCnt, srcCompCost, workerCompCostMax, workerCompCostSum);

            gettimeofday(&endTV, NULL);

            timersub(&endTV, &startTV, &diff);

            double elapsedTime = diff.tv_sec * 1000 + diff.tv_usec / 1000 ;

        } else {

            double srcCompCost = 0;
            double workerCompCostMax = 0;
            double workerCompCostSum = 0;
            std::vector<float> nodeToCnt;
            //run_mpi_pes(input, hIO, workerNum, memSize, bufLen, tolerance, seed + repeat * workerNum * i, nodeToCnt, srcCompCost, workerCompCostMax, workerCompCostSum);
        }
    }
}

