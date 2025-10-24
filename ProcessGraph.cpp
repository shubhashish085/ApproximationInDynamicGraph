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
    }

    std::sort(accuracy_array, accuracy_array + trial_count);
    ui median_idx = trial_count / 2;

    std::cout << "Accuracy : " << accuracy_array[median_idx] << std::endl; 
}

void loadFullyDynamicGraphStreamForMascot(const std::string& file_path, MascotFD*& module, Graph*& data_graph, ui interval, 
                                long long*& exact_count, double*& global_cnt, double*& error_array, ui& serial){
    
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

    VertexID begin, end;
    std::string addition;

    ui approximated_count = 0, interval_counter = 0, trial_counter = 0;

    double max_error = 0.0, min_error = 30.0;

    std::cout << "Ignoring the comments... " << std::endl;

    while(infile >> begin) {

        infile >> end;
        infile >> addition;

        if(addition == "-"){
            module-> processEdge(begin, end, false);
            //data_graph->delete_edge(begin, end);

        }else{
            module-> processEdge(begin, end, true);
            //data_graph->add_edge(begin, end);
        }

        
        interval_counter++;

        if(interval_counter >= interval){           

            //exact_count[serial] = data_graph->alt_count_exact_triangle();

            global_cnt[serial] = module->getGlobalTriangle();

            std::cout << "Global Triangle Count : " << global_cnt[serial] << std::endl;

            //error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);

            /*if(max_error < error_array[serial]){
                max_error = error_array[serial];
            }

            if(min_error > error_array[serial]){
                min_error = error_array[serial];
            }*/

            interval_counter = 0;
            serial++;            
        }
    }

    std::cout << "Maximum Error : " << max_error << std::endl;
    std::cout << "Minimum Error : " << min_error << std::endl;

    infile.close();
}


void loadGraphByStreamForMascot(const std::string& file_path, MascotFD*& module, Graph*& data_graph, ui interval, 
                                long long*& exact_count, double*& global_cnt, double*& error_array, ui& serial){

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

    double max_error = 0.0, min_error = 30.0;

    std::cout << "Ignoring the comments... " << std::endl;

    while(infile >> begin) {

        infile >> end;
        module-> processEdge(begin, end, true);
        data_graph->add_edge(begin, end);

        
        interval_counter++;

        if(interval_counter >= interval){

            trial_counter++;            

            exact_count[serial] = data_graph->alt_count_exact_triangle();

            global_cnt[serial] = module->getGlobalTriangle();

            error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);

            if(max_error < error_array[serial]){
                max_error = error_array[serial];
            }

            if(min_error > error_array[serial]){
                min_error = error_array[serial];
            }

            //exact_triangle_cnt = data_graph->count_exact_triangle();
            //exact_triangle_cnt = data_graph->alt_count_exact_triangle();

            //error = (double) ((exact_triangle_cnt - module->getGlobalTriangle())) / exact_triangle_cnt;

            //std::cout << "Trial : " << trial_counter << "  Exact Number of Triangles : " << exact_triangle_cnt << std::endl;

            //get_metric(exact_triangle_cnt, module->getGlobalTriangle(), trial_counter);

            interval_counter = 0;
            serial++;            
        }
    }

    std::cout << "Maximum Error : " << max_error << std::endl;
    std::cout << "Minimum Error : " << min_error << std::endl;

    infile.close();

}

void loadFullyDynamicGraphByStreamForTriest(const std::string& file_path, TriestFD*& module, Graph*& data_graph, ui interval,
                                long long*& exact_count, double*& global_cnt, double*& error_array, ui& serial){
    
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

    
    VertexID begin, end;
    std::string addition;

    ui approximated_count = 0, interval_counter = 0, trial_counter = 0;

    double max_error = 0.0, min_error = 30.0;

    while(infile >> begin) {

        infile >> end;
        infile >> addition;

        if(addition == "-"){
            module-> processEdge(begin, end, false);
            //data_graph->delete_edge(begin, end);

        }else{
            module-> processEdge(begin, end, true);
            //data_graph->add_edge(begin, end);
        }
        
        interval_counter++;

        if(interval_counter >= interval){
             
            //exact_count[serial] = data_graph->alt_count_exact_triangle();

            global_cnt[serial] = module->getGlobalTriangle();

            //std::cout << "Global Triangle Count : " << global_cnt[serial] << std::endl;

            //error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);
            

            /*if(max_error < error_array[serial]){
                max_error = error_array[serial];
            }

            if(min_error > error_array[serial]){
                min_error = error_array[serial];
            }*/

            interval_counter = 0;
            serial++;

            
        }
    }

    std::cout << "Final Triangle : " << module->getGlobalTriangle() << std::endl;

    std::cout << "Maximum Error : " << max_error << std::endl;
    std::cout << "Minimum Error : " << min_error << std::endl;

    infile.close();
}

void loadIncrementalGraphByStreamForTriest(const std::string& file_path, TriestFD*& module, Graph*& data_graph, ui interval,
                                long long*& exact_count, double*& global_cnt, double*& error_array, ui& serial){

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
    std::string addition;

    ui approximated_count = 0, interval_counter = 0, trial_counter = 0;

    double max_error = 0.0, min_error = 30.0;

    while(infile >> begin) {

        infile >> end;
        infile >> addition;

        module-> processEdge(begin, end, true);
        data_graph->add_edge(begin, end);
        
        interval_counter++;

        if(interval_counter >= interval && serial <= 20){

            trial_counter++;
             
            //exact_count[serial] = data_graph->alt_count_exact_triangle();

            global_cnt[serial] = module->getGlobalTriangle();

            std::cout << "Global Triangle Count : " << global_cnt[serial] << std::endl;

            //error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);
            

            if(max_error < error_array[serial]){
                max_error = error_array[serial];
            }

            if(min_error > error_array[serial]){
                min_error = error_array[serial];
            }

            interval_counter = 0;
            serial++;
        }
    }

    std::cout << "Maximum Error : " << max_error << std::endl;
    std::cout << "Minimum Error : " << min_error << std::endl;

    infile.close();
}

void loadFullyDynamicGraphStreamForThinkD(const std::string& file_path, ThinkDFD*& module, Graph*& data_graph, ui interval,
                                    long long*& exact_count, double*& global_cnt, double*& error_array, ui& serial){
    
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

    VertexID begin, end;
    std::string addition;

    ui approximated_count = 0, interval_counter = 0, trial_counter = 0;

    double max_error = 0.0, min_error = 30.0;

    while(infile >> begin) {

        infile >> end;
        infile >> addition;

        if(addition == "-"){
            module-> processEdge(begin, end, false);
            //data_graph->delete_edge(begin, end);

        }else{
            module-> processEdge(begin, end, true);
            //data_graph->add_edge(begin, end);
        }

        
        interval_counter++;

        if(interval_counter >= interval){

            //exact_count[serial] = data_graph->alt_count_exact_triangle();

            global_cnt[serial] = module->getGlobalTriangle();

            /*error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);

            if(max_error < error_array[serial]){
                max_error = error_array[serial];
            }

            if(min_error > error_array[serial]){
                min_error = error_array[serial];
            }*/

            interval_counter = 0;
            serial++;
        }
    }

    std::cout << "Maximum Error : " << max_error << std::endl;
    std::cout << "Minimum Error : " << min_error << std::endl;

    infile.close();
}


void loadIncrementalGraphByStreamForThinkD(const std::string& file_path, ThinkDFD*& module, Graph*& data_graph, ui interval,
                                    long long*& exact_count, double*& global_cnt, double*& error_array, ui& serial){

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

    double max_error = 0.0, min_error = 30.0;

    while(infile >> begin) {

        infile >> end;
        module-> processEdge(begin, end, true);
        data_graph->add_edge(begin, end);

        
        interval_counter++;

        if(interval_counter >= interval){

            trial_counter++;

            exact_count[serial] = data_graph->alt_count_exact_triangle();

            global_cnt[serial] = module->getGlobalTriangle();

            error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);

            if(max_error < error_array[serial]){
                max_error = error_array[serial];
            }

            if(min_error > error_array[serial]){
                min_error = error_array[serial];
            }

            //exact_triangle_cnt = data_graph-> alt_count_exact_triangle();

            //get_metric(exact_triangle_cnt, module->getGlobalTriangle(), trial_counter);

            interval_counter = 0;
            serial++;
        }
    }

    std::cout << "Maximum Error : " << max_error << std::endl;
    std::cout << "Minimum Error : " << min_error << std::endl;

    infile.close();
}


void write_into_output_file(std::string output_file_path, long long* exact_cnt_array, double* global_cnt, double* error, ui serial_cnt){


    std::ofstream outputfile;
    outputfile.open(output_file_path, std::ios::app);

    outputfile << "Serial" << "  " << "Exact_Count" << "  " << "Global_Count" << "  " << "Error" << std::endl;

    for (ui i = 0; i < serial_cnt; i++){
        outputfile << i << "  " << exact_cnt_array[i] << "  " << global_cnt[i] << "  " << error[i] << std::endl;

        if(i % 1000 == 0){
            outputfile.flush();
        }

    }

    outputfile.flush();
    outputfile.close();
}

void print_details(std::string input_graph_file, std::string algorithm_serial, std::string memory_budget, ui interval){

    std::cout << "Input File : " << input_graph_file << std::endl;
    std::cout << "Algorithm Serial : " << algorithm_serial << std::endl;
    std::cout << "Memory Budget : " << memory_budget << std::endl;
    std::cout << "Interval : " << interval << std::endl;
}



//TriestFD
/*int main(int argc, char** argv){

    //std::string input_data_graph_file = "/home/kars1/Parallel_computation/dataset/com-dblp.ungraph.txt";
    std::string input_data_graph_file = "./com-amazon_stm_5d.ungraph.txt";

    ui memory_budget = 65536;
    bool lowerbound = true;
    ui interval = 1000, serial_cnt = 0; 

    ui max_array_limit = 100000;

    long long* exact_cnt_array = new long long[max_array_limit];
    double* global_cnt_array = new double[max_array_limit];
    double* error_array = new double[max_array_limit];

    Graph* data_graph = new Graph();

    TriestFD* module = new TriestFD(memory_budget, lowerbound);
    loadFullyDynamicGraphByStreamForTriest(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);
    //loadIncrementalGraphByStreamForTriest(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);        
}*/


//MascotFD
int main(int argc, char** argv){

    //std::string input_data_graph_file = "/home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt";
    std::string input_data_graph_file = "./com-amazon_stm_5d.ungraph.txt";

    ui memory_budget = 65536;
    double sample_probability = 0.1;
    bool lowerbound = true;
    ui interval = 1000, serial_cnt = 0;

    ui max_array_limit = 100000;

    long long* exact_cnt_array = new long long[max_array_limit];
    double* global_cnt_array = new double[max_array_limit];
    double* error_array = new double[max_array_limit];


    Graph* data_graph = new Graph();

    MascotFD* module = new MascotFD(memory_budget, sample_probability, lowerbound);
    //loadGraphByStreamForMascot(input_data_graph_file, module, data_graph, interval);
    loadFullyDynamicGraphStreamForMascot(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);
}


//ThinkDFD
/*int main(int argc, char** argv){

    //std::string input_data_graph_file = "/home/kars1/Parallel_computation/dataset/com-amazon.ungraph.txt";
    std::string input_data_graph_file = "./com-amazon_stm_10d.ungraph.txt";

    ui memory_budget = 65536;
    bool lowerbound = true;
    ui interval = 1000, serial_cnt = 0; 

    ui max_array_limit = 100000;

    long long* exact_cnt_array = new long long[max_array_limit];
    double* global_cnt_array = new double[max_array_limit];
    double* error_array = new double[max_array_limit];   

    Graph* data_graph = new Graph();

    ThinkDFD* module = new ThinkDFD(memory_budget, lowerbound);
    //loadGraphByStreamForThinkD(input_data_graph_file, module, data_graph, interval);
    loadFullyDynamicGraphStreamForThinkD(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);
}*/




//Incremental
/*int main(int argc, char** argv){

    MatchingCommand command(argc, argv);
    
    std::string input_data_graph_file = command.getDataGraphFilePath();
    std::string output_file = command.getOutputFilePath();
    std::string algorithm_serial = command.getAlgorithmSerial();
    std::string memory_budget_str = command.getMemoryBudget();
    std::string sampling_prob_str = command.getSamplingProbability();

    ui memory_budget = std::stoi(memory_budget_str);;
    bool lowerbound = true;
    ui interval = 1000, serial_cnt = 0;

    ui max_array_limit = 100000;

    long long* exact_cnt_array = new long long[max_array_limit];
    double* global_cnt_array = new double[max_array_limit];
    double* error_array = new double[max_array_limit];


    Graph* data_graph = new Graph();

    if(algorithm_serial == "1"){
        double sample_probability = std::stod(sampling_prob_str);

        MascotFD* module = new MascotFD(memory_budget, sample_probability, lowerbound);
        loadGraphByStreamForMascot(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);        
    }else if(algorithm_serial == "2"){
        
        TriestFD* module = new TriestFD(memory_budget, lowerbound);
        loadGraphByStreamForTriest(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);        
    }else {

        ThinkDFD* module = new ThinkDFD(memory_budget, lowerbound);
        loadGraphByStreamForThinkD(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);        
    }

    print_details(input_data_graph_file, algorithm_serial, memory_budget_str, interval);
    write_into_output_file(output_file, exact_cnt_array, global_cnt_array, error_array, serial_cnt);
}*/

// Fully Dynamic Setting
/*int main(int argc, char** argv){

    MatchingCommand command(argc, argv);
    
    std::string input_data_graph_file = command.getDataGraphFilePath();
    std::string output_file = command.getOutputFilePath();
    std::string algorithm_serial = command.getAlgorithmSerial();
    std::string memory_budget_str = command.getMemoryBudget();
    std::string sampling_prob_str = command.getSamplingProbability();

    ui memory_budget = std::stoi(memory_budget_str);;
    bool lowerbound = true;
    ui interval = 1000, serial_cnt = 0;

    ui max_array_limit = 100000;

    long long* exact_cnt_array = new long long[max_array_limit];
    double* global_cnt_array = new double[max_array_limit];
    double* error_array = new double[max_array_limit];


    Graph* data_graph = new Graph();

    if(algorithm_serial == "1"){
        double sample_probability = std::stod(sampling_prob_str);

        MascotFD* module = new MascotFD(memory_budget, sample_probability, lowerbound);
        loadFullyDynamicGraphStreamForMascot(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);        
    }else if(algorithm_serial == "2"){
        
        TriestFD* module = new TriestFD(memory_budget, lowerbound);
        loadFullyDynamicGraphByStreamForTriest(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);        
    }else {

        ThinkDFD* module = new ThinkDFD(memory_budget, lowerbound);
        loadFullyDynamicGraphStreamForThinkD(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);
    }

    print_details(input_data_graph_file, algorithm_serial, memory_budget_str, interval);
    write_into_output_file(output_file, exact_cnt_array, global_cnt_array, error_array, serial_cnt);
}*/






