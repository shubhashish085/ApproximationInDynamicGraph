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

    std::sort(error, error + serial_cnt);
    int median_idx = serial_cnt / 2;

    std::cout << "Median Error : " << error[median_idx] << std::endl;
    std::cout << "-----------------------------------" << std::endl; 

    outputfile.flush();
    outputfile.close();
}

void print_details(std::string input_graph_file, std::string algorithm_serial, std::string memory_budget, ui interval){

    std::cout << "Input File : " << input_graph_file << std::endl;
    std::cout << "Algorithm Serial : " << algorithm_serial << std::endl;
    std::cout << "Memory Budget : " << memory_budget << std::endl;
    std::cout << "Interval : " << interval << std::endl;
}

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
            data_graph->delete_edge(begin, end);

        }else{
            module-> processEdge(begin, end, true);
            data_graph->add_edge(begin, end);
        }

        
        interval_counter++;

        if(interval_counter >= interval){           

            exact_count[serial] = data_graph->get_global_triangle_count();

            global_cnt[serial] = module->getGlobalTriangle();

            error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);

            //std::cout << "Error : " << error_array[serial] << std::endl;

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


void loadIncrementalGraphByStreamForMascot(const std::string& file_path, MascotFD*& module, Graph*& data_graph, ui interval, 
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

            exact_count[serial] = data_graph->get_global_triangle_count();

            global_cnt[serial] = module->getGlobalTriangle();

            error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);

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


void loadIncrementalGraphByStreamForPES(const std::string& file_path, PES*& module, Graph*& data_graph, ui interval, 
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

            exact_count[serial] = data_graph->get_global_triangle_count();

            global_cnt[serial] = module->getGlobalTriangle();

            error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);

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
            data_graph->delete_edge(begin, end);

        }else{
            module-> processEdge(begin, end, true);
            data_graph->add_edge(begin, end);
        }
        
        interval_counter++;

        if(interval_counter >= interval){
             
            exact_count[serial] = data_graph->get_global_triangle_count();

            global_cnt[serial] = module->getGlobalTriangle();

            error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);
            
            //std::cout << "Error  : " << error_array[serial] << std::endl;

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
             
            exact_count[serial] = data_graph->get_global_triangle_count();

            global_cnt[serial] = module->getGlobalTriangle();

            error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);
            

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
            data_graph->delete_edge(begin, end);

        }else{
            module-> processEdge(begin, end, true);
            data_graph->add_edge(begin, end);
        }

        
        interval_counter++;

        if(interval_counter >= interval){

            exact_count[serial] = data_graph->get_global_triangle_count();

            global_cnt[serial] = module->getGlobalTriangle();

            error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);

            //std::cout << "Error : " << error_array[serial] << std::endl;

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


void countSquareForThinkDInIncrementalGraphStream(const std::string& file_path, const std::string& output_file, ThinkDFD*& module, Graph*& data_graph, ui interval){
    
    std::ifstream infile(file_path);
    std::ofstream outputfile;
    outputfile.open(output_file, std::ios::app);

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

        module-> processEdgeSquare(begin, end, true);

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




void countSquareForThinkDInFullyDynamicGraphStream(const std::string& file_path, const std::string& output_file, ThinkDFD*& module, Graph*& data_graph, ui interval){
    
    std::ifstream infile(file_path);
    std::ofstream outputfile;
    outputfile.open(output_file, std::ios::app);

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


void countButterflyForThinkDInFullyDynamicGraphStream(const std::string& file_path, const std::string& output_file, ThinkDFD*& module, Graph*& data_graph, ui interval,
                                    long long*& exact_count, double*& global_cnt, double*& error_array, ui& serial){
    
    std::ifstream infile(file_path);
    std::ofstream outputfile;
    outputfile.open(output_file, std::ios::app);


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
            module-> processEdgeButterfly(begin, end, false);
        }else{
            module-> processEdgeButterfly(begin, end, true);
        }

        interval_counter++;

        if(interval_counter % interval == 0){

            outputfile << interval_counter << "  " << module->getGlobalButterfly() << std::endl;
            outputfile.flush();

        }
        
    }

    infile.close();
    outputfile.close();

    //write_into_output_file(output_file, exact_count, global_cnt, error_array, serial);
}


void printExactButterflyCount(const std::string& file_path, const std::string& output_file, Graph*& data_graph, ui step){

    std::ifstream infile(file_path);
    std::ofstream outputfile;
    outputfile.open(output_file, std::ios::app);


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

    ui  interval_counter = 0;

    while(infile >> begin) {

        infile >> end;
        infile >> addition;

        if(addition == "-"){
            data_graph->delete_edge_butterfly(begin, end);

        }else{
            data_graph->add_edge_butterfly(begin, end);
        }

        interval_counter++;

        if(interval_counter % step == 0){
            outputfile << interval_counter << "  " << data_graph -> get_global_butterfly_count() << std::endl;
            outputfile.flush();
        }       
    }

    infile.close();
    outputfile.flush();
    outputfile.close();
}

void printExactSquareCount(const std::string& file_path, const std::string& output_file, Graph*& data_graph, ui step){

    std::ifstream infile(file_path);
    std::ofstream outputfile;
    outputfile.open(output_file, std::ios::app);


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

    ui  interval_counter = 0;

    while(infile >> begin) {

        infile >> end;
        infile >> addition;

        if(addition == "-"){
            data_graph->delete_edge_square(begin, end);

        }else{
            data_graph->add_edge_square(begin, end);
        }

        interval_counter++;

        if(interval_counter % step == 0){
            outputfile << interval_counter << "  " << data_graph -> get_global_square_count() << std::endl;
            outputfile.flush();
        }       
    }

    infile.close();
    outputfile.flush();
    outputfile.close();
}


void printErrorDetails(const std::string& input_file_1, const std::string& input_file_2){

    std::ifstream infile1(input_file_1);
    std::ifstream infile2(input_file_2);

    std::vector<double> exact_cnt_vtr;
    std::vector<double> approx_cnt_vtr;

    if (!infile1.is_open()) {
        std::cout << "Can not open the graph file " << input_file_1 << " ." << std::endl;
        exit(-1);
    }

    if (!infile2.is_open()) {
        std::cout << "Can not open the graph file " << input_file_2 << " ." << std::endl;
        exit(-1);
    }

    ui counter = 0;

    ui edge_counter_1, edge_counter_2;
    double bfy_count_1, bfy_count_2; 

    while(infile1 >> edge_counter_1){

        infile1 >> bfy_count_1;

        exact_cnt_vtr.push_back(bfy_count_1);

        if(infile2 >> edge_counter_2){
            infile2 >> bfy_count_2;
            approx_cnt_vtr.push_back(bfy_count_2);
        }
    }

    infile1.close();
    infile2.close();

    double error, max_error = 0.0, min_error = 30.0;

    for(ui i = 0; i < exact_cnt_vtr.size(); i++){

        error = std::abs((exact_cnt_vtr[i] - approx_cnt_vtr[i]) / exact_cnt_vtr[i]);

        if(max_error < error){
            max_error = error;
        }

        if(min_error > error){
            min_error = error;
        }
    }
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

            exact_count[serial] = data_graph->get_global_triangle_count();

            global_cnt[serial] = module->getGlobalTriangle();

            error_array[serial] = std::abs((double) ((exact_count[serial] - module->getGlobalTriangle()) * 100.0) / exact_count[serial]);

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
/*int main(int argc, char** argv){

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
}*/


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

//For Butterfly
/*
int main(int argc, char** argv){

    MatchingCommand command(argc, argv);
    
    std::string input_data_graph_file = command.getDataGraphFilePath();
    std::string output_file = command.getOutputFilePath();
    std::string memory_budget_str = command.getMemoryBudget();

    ui memory_budget = 65536;
    bool lowerbound = true;
    ui interval = 2000, serial_cnt = 0; 

    ui max_array_limit = 100000;


    std::cout << "Input File : " << input_data_graph_file << std::endl;    
    std::cout << "Memory Budget : " << memory_budget << std::endl;
    std::cout << "Interval : " << interval << std::endl;

    long long* exact_cnt_array = new long long[max_array_limit];
    double* global_cnt_array = new double[max_array_limit];
    double* error_array = new double[max_array_limit];   

    Graph* data_graph = new Graph();

    ThinkDFD* module = new ThinkDFD(memory_budget, lowerbound);
    //loadGraphByStreamForThinkD(input_data_graph_file, module, data_graph, interval);
    //loadFullyDynamicGraphStreamForThinkD(input_data_graph_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);
    //countSquareForThinkDInFullyDynamicGraphStream(input_data_graph_file, output_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);
    countButterflyForThinkDInFullyDynamicGraphStream(input_data_graph_file, output_file, module, data_graph, interval, exact_cnt_array, global_cnt_array, error_array, serial_cnt);
}*/


//For Square
/*int main(int argc, char** argv){

    MatchingCommand command(argc, argv);
    
    std::string input_data_graph_file = command.getDataGraphFilePath();
    std::string output_file = command.getOutputFilePath();
    std::string memory_budget_str = command.getMemoryBudget();

    ui memory_budget = 65536;
    bool lowerbound = true;
    ui interval = 2000, serial_cnt = 0; 

    ui max_array_limit = 100000;


    std::cout << "Input File : " << input_data_graph_file << std::endl;    
    std::cout << "Memory Budget : " << memory_budget << std::endl;
    std::cout << "Interval : " << interval << std::endl;

    long long* exact_cnt_array = new long long[max_array_limit];
    double* global_cnt_array = new double[max_array_limit];
    double* error_array = new double[max_array_limit];   

    Graph* data_graph = new Graph();

    ThinkDFD* module = new ThinkDFD(memory_budget, lowerbound);    
    countSquareForThinkDInFullyDynamicGraphStream(input_data_graph_file, output_file, module, data_graph, interval);
    
}*/


/*int main(int argc, char** argv){

    MatchingCommand command(argc, argv);
    
    std::string input_data_graph_file = command.getDataGraphFilePath();
    std::string output_file = command.getOutputFilePath();
    std::string step_count = command.getStepCount();

    ui step = std::stoi(step_count);

    Graph* data_graph = new Graph();

    //printExactButterflyCount(input_data_graph_file, output_file, data_graph, 2000);
    printExactSquareCount(input_data_graph_file, output_file, data_graph, step);
}*/