#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include "graph.h"
#include "types.h"
#include "MatchingCommand.h"
#include "MascotFD.hpp"
#include "ThinkDFD.hpp"
#include "TriestFD.hpp"


void get_metric(long long exact_count, double approximated_count, ui serial){

    double error = 0.0;
    double avg_elapsed_time = 0;

    error = (double) ((exact_count - approximated_count)) / exact_count;    

    std::cout << "Exact Count : " << exact_count << std::endl;
    std::cout << "Approximated Count : " << approximated_count << std::endl;
    std::cout << "Error : " << error << std::endl;     
}



void get_metrics(long long* exact_count_array, long long* approximated_count_array, double* elapsed_time_array, ui trial_count){

    double* accuracy_array = new double[trial_count];
    double avg_elapsed_time = 0;

    for(ui i = 0; i < trial_count; i++){
        accuracy_array[i] = (double) (std::abs(exact_count_array[i] - approximated_count_array[i])) / exact_count_array[i];
        //avg_elapsed_time += elapsed_time_array[i];
    }

    std::sort(accuracy_array, accuracy_array + trial_count);
    ui median_idx = trial_count / 2;
    //avg_elapsed_time = avg_elapsed_time / trial_count;

    std::cout << "Accuracy : " << accuracy_array[median_idx] << std::endl;
    //std::cout << "Elapsed Time : " << avg_elapsed_time << std::endl;   
}



void loadGraphByStreamForMascot(const std::string& file_path, MascotFD*& module, Graph*& data_graph, ui interval){

    std::ifstream infile(file_path);
    long long exact_triangle_cnt = 0;

    if (!infile.is_open()) {
        std::cout << "Can not open the graph file " << file_path << " ." << std::endl;
        exit(-1);
    }

    char type;
    std::string input_line;
    ui label = 0;

    std::cout << "Reading File............ " << std::endl;

    ui line_count = 0, count = 0, comment_line_count = 4;

    while (std::getline(infile, input_line)) {

        line_count++;

        if(line_count >= comment_line_count){
            break;
        }
    }

    VertexID begin, end;

    ui approximated_count = 0, interval_counter = 0, trial_counter = 0;

    std::cout << "Ignoring the comments... " << std::endl;

    while(infile >> begin) {

        infile >> end;
        module-> processEdge(begin, end, true);
        data_graph->add_edge(begin, end);

        
        interval_counter++;

        if(interval_counter >= interval){

            trial_counter++;
            
            //exact_triangle_cnt = data_graph->count_exact_triangle();
            exact_triangle_cnt = data_graph->alt_count_exact_triangle();

            std::cout << "Trial : " << trial_counter << "  Exact Number of Triangles : " << exact_triangle_cnt << std::endl;

            get_metric(exact_triangle_cnt, module->getGlobalTriangle(), trial_counter);

            interval_counter = 0;
            
        }
    }

    infile.close();

    std::ifstream input_file(file_path);

}

void loadGraphByStreamForTriest(const std::string& file_path, TriestFD*& module, Graph*& data_graph, ui interval){

    std::ifstream infile(file_path);
    long long exact_triangle_cnt = 0;

    if (!infile.is_open()) {
        std::cout << "Can not open the graph file " << file_path << " ." << std::endl;
        exit(-1);
    }

    char type;
    std::string input_line;
    ui label = 0;

    std::cout << "Reading File............ " << std::endl;

    ui line_count = 0, count = 0, comment_line_count = 4;

    while (std::getline(infile, input_line)) {

        line_count++;

        if(line_count >= comment_line_count){
            break;
        }
    }

    VertexID begin, end;

    ui approximated_count = 0, interval_counter = 0, trial_counter = 0;

    while(infile >> begin) {

        infile >> end;
        module-> processEdge(begin, end, true);
        data_graph->add_edge(begin, end);

        
        interval_counter++;

        if(interval_counter >= interval){

            trial_counter++;

            exact_triangle_cnt = data_graph->alt_count_exact_triangle();

            get_metric(exact_triangle_cnt, module->getGlobalTriangle(), trial_counter);

            interval_counter = 0;
        }
    }

    infile.close();

    std::ifstream input_file(file_path);

}


void loadGraphByStreamForThinkD(const std::string& file_path, ThinkDFD*& module, Graph*& data_graph, ui interval){

    std::ifstream infile(file_path);
    long long exact_triangle_cnt = 0;

    if (!infile.is_open()) {
        std::cout << "Can not open the graph file " << file_path << " ." << std::endl;
        exit(-1);
    }

    char type;
    std::string input_line;
    ui label = 0;

    std::cout << "Reading File............ " << std::endl;

    ui line_count = 0, count = 0, comment_line_count = 4;

    while (std::getline(infile, input_line)) {

        line_count++;

        if(line_count >= comment_line_count){
            break;
        }
    }

    VertexID begin, end;

    ui approximated_count = 0, interval_counter = 0, trial_counter = 0;

    while(infile >> begin) {

        infile >> end;
        module-> processEdge(begin, end, true);
        data_graph->add_edge(begin, end);

        
        interval_counter++;

        if(interval_counter >= interval){

            trial_counter++;

            exact_triangle_cnt = data_graph-> alt_count_exact_triangle();

            get_metric(exact_triangle_cnt, module->getGlobalTriangle(), trial_counter);

            interval_counter = 0;
        }
    }

    infile.close();

    std::ifstream input_file(file_path);
}




//TriestFD
int main(int argc, char** argv){

    std::string input_data_graph_file = "/home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt";

    ui memory_budget = 65536;
    bool lowerbound = true;
    ui interval = 1000;

    Graph* data_graph = new Graph();

    TriestFD* module = new TriestFD(memory_budget, lowerbound);
    loadGraphByStreamForTriest(input_data_graph_file, module, data_graph, interval);
}


//MascotFD
/*int main(int argc, char** argv){

    std::string input_data_graph_file = "/home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt";

    ui memory_budget = 65536;
    double sample_probability = 0.05;
    bool lowerbound = true;
    ui interval = 1000;

    Graph* data_graph = new Graph();

    MascotFD* module = new MascotFD(memory_budget, sample_probability, lowerbound);
    loadGraphByStreamForMascot(input_data_graph_file, module, data_graph, interval);
}*/


//ThinkDFD
/*int main(int argc, char** argv){

    std::string input_data_graph_file = "/home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt";

    ui memory_budget = 65536;
    bool lowerbound = true;
    ui interval = 1000;    

    Graph* data_graph = new Graph();

    ThinkDFD* module = new ThinkDFD(memory_budget, lowerbound);
    loadGraphByStreamForThinkD(input_data_graph_file, module, data_graph, interval);
}*/

/*
int main(int argc, char** argv){

    MatchingCommand command(argc, argv);
    
    std::string input_data_graph_file = command.getDataGraphFilePath();
    std::string output_file = command.getOutputFilePath();
    std::string algorithm_serial = command.getAlgorithmSerial();
    std::string memory_budget_str = command.getMemoryBudget();
    std::string sampling_prob_str = command.getSamplingProbability();

    ui memory_budget = std::stoi(memory_budget_str);;
    bool lowerbound = true;
    ui interval = 1000;

    Graph* data_graph = new Graph();

    if(algorithm_serial == "1"){
        double sample_probability = std::stod(sampling_prob_str);

        MascotFD* module = new MascotFD(memory_budget, sample_probability, lowerbound);
        loadGraphByStreamForMascot(input_data_graph_file, module, data_graph, interval);        
    }else if(algorithm_serial == "2"){
        
        TriestFD* module = new TriestFD(memory_budget, lowerbound);
        loadGraphByStreamForTriest(input_data_graph_file, module, data_graph, interval);        
    }else {

        ThinkDFD* module = new ThinkDFD(memory_budget, lowerbound);
        loadGraphByStreamForThinkD(input_data_graph_file, module, data_graph, interval);        
    }
}

*/


