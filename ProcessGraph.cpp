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

void loadGraphByStreamForMascot(const std::string& file_path, MascotFD*& module, Graph*& data_graph){

    std::ifstream infile(file_path);

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

    ui approximated_count = 0, interval = 10, interval_counter = 0;

    while(infile >> begin) {

        infile >> end;
        module-> processEdge(begin, end, true);
        data_graph->add_edge(begin, end);

        
        interval_counter++;

        if(interval_counter >= interval){
            
            interval_counter = 0;
        }
    }

    infile.close();

    std::ifstream input_file(file_path);

}

void loadGraphByStreamForTriest(const std::string& file_path, TriestFD*& module, Graph*& data_graph){

    std::ifstream infile(file_path);

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

    ui approximated_count = 0, interval = 10, interval_counter = 0;

    while(infile >> begin) {

        infile >> end;
        module-> processEdge(begin, end, true);
        data_graph->add_edge(begin, end);

        
        interval_counter++;

        if(interval_counter >= interval){
            
            interval_counter = 0;
        }
    }

    infile.close();

    std::ifstream input_file(file_path);

}


void loadGraphByStreamForThinkD(const std::string& file_path, ThinkDFD*& module, Graph*& data_graph){

    std::ifstream infile(file_path);

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

    ui approximated_count = 0, interval = 10, interval_counter = 0;

    while(infile >> begin) {

        infile >> end;
        module-> processEdge(begin, end, true);
        data_graph->add_edge(begin, end);

        
        interval_counter++;

        if(interval_counter >= interval){
            
            interval_counter = 0;
        }
    }

    infile.close();

    std::ifstream input_file(file_path);

}

void get_metrics(long long exact_count, long long* approximated_count_array, double* elapsed_time_array, ui trial_count){

    double* accuracy_array = new double[trial_count];
    double avg_elapsed_time = 0;

    for(ui i = 0; i < trial_count; i++){
        accuracy_array[i] = (double) (std::abs(exact_count - approximated_count_array[i])) / exact_count;
        avg_elapsed_time = elapsed_time_array[i];
    }

    std::sort(accuracy_array, accuracy_array + trial_count);
    ui median_idx = trial_count / 2;
    avg_elapsed_time = avg_elapsed_time / trial_count;

    std::cout << "Accuracy : " << accuracy_array[median_idx] << std::endl;
    std::cout << "Elapsed Time : " << avg_elapsed_time << std::endl;   
}

//TriestFD
/*int main(int argc, char** argv){

    std::string input_data_graph_file = "/home/antu/Research_Projects/dataset/com-amazon.ungraph.txt";

    ui memory_budget;
    double sample_probability = 0.3;
    bool lowerbound = true;

    Graph* data_graph = new Graph();

    TriestFD* module = new TriestFD(memory_budget, lowerbound);

    loadGraphByStreamForTriest(input_data_graph_file, module, data_graph);
}*/


//MascotFD
/*int main(int argc, char** argv){

    std::string input_data_graph_file = "/home/antu/Research_Projects/dataset/com-amazon.ungraph.txt";

    ui memory_budget;
    double sample_probability = 0.3;
    bool lowerbound = true;

    Graph* data_graph = new Graph();

    MascotFD* module = new MascotFD(memory_budget, sample_probability, lowerbound);

    loadGraphByStreamForMascot(input_data_graph_file, module, data_graph);
}*/


//ThinkDFD
/*int main(int argc, char** argv){

    std::string input_data_graph_file = "/home/antu/Research_Projects/dataset/com-amazon.ungraph.txt";

    ui memory_budget;
    bool lowerbound = true;    

    Graph* data_graph = new Graph();

    ThinkDFD* module = new ThinkDFD(memory_budget, lowerbound);

    loadGraphByStreamForThinkD(input_data_graph_file, module, data_graph);
}*/


int main(int argc, char** argv){

    MatchingCommand command(argc, argv);
    
    std::string input_data_graph_file = command.getDataGraphFilePath();
    std::string output_file = command.getOutputFilePath();
    std::string algorithm_serial = command.getAlgorithmSerial();
    std::string memory_budget_str = command.getMemoryBudget();
    std::string sampling_prob_str = command.getSamplingProbability();

    ui memory_budget = std::stoi(memory_budget_str);;
    bool lowerbound = true;

    Graph* data_graph = new Graph();

    if(algorithm_serial == "1"){
        double sample_probability = std::stod(sampling_prob_str);

        MascotFD* module = new MascotFD(memory_budget, sample_probability, lowerbound);
        loadGraphByStreamForMascot(input_data_graph_file, module, data_graph);        
    }else if(algorithm_serial == "2"){
        
        TriestFD* module = new TriestFD(memory_budget, lowerbound);
        loadGraphByStreamForTriest(input_data_graph_file, module, data_graph);        
    }else {

        ThinkDFD* module = new ThinkDFD(memory_budget, lowerbound);
        loadGraphByStreamForThinkD(input_data_graph_file, module, data_graph);        
    }
}


