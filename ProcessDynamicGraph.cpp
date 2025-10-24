#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <set>
#include "types.h"


void create_uniform_random_deletion_indices(ui length, ui deletion_count, std::set<ui>& random_indices){

    ui min_index = 0, max_index = length - 1;

    std::random_device rd;
    std::mt19937 generator(rd()); 

    std::uniform_int_distribution<> distribution(min_index, max_index);

    for (int i = 0; i < deletion_count; ++i) {
        random_indices.insert(distribution(generator));
    }
}


void create_streaming_graph_from_file(const std::string& file_path, const std::string& output_file_path, ui deletion_percentage){

    std::ifstream infile(file_path);
    std::vector<std::pair<VertexID, VertexID>> edge_list;
    std::vector<std::pair<VertexID, VertexID>> final_edge_list;
    std::set<ui> deletion_indices;

    if (!infile.is_open()) {
        std::cout << "Can not open the graph file " << file_path << " ." << std::endl;
        exit(-1);
    }

    char type;
    std::string input_line;

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
        edge_list.push_back(std::make_pair(begin, end));
    }

    infile.close();

    std::cout << "Edges Count : " << edge_list.size() << std::endl;

    ui deletion_count = std::floor(edge_list.size() * deletion_percentage / 100);

    create_uniform_random_deletion_indices(edge_list.size(), deletion_count, deletion_indices);

    ui total_size = edge_list.size() + deletion_indices.size();

    bool* addition_sign_array = new bool[total_size];

    ui start_idx = 0, end_idx, idx = 0;

    std::cout << "Edges To Be Deleted : " << deletion_indices.size() << std::endl;

    for(ui deletion_idx : deletion_indices){
        end_idx = deletion_idx;       
        for(ui i = start_idx; i <= end_idx; i++){
            final_edge_list.push_back(edge_list[i]);
            addition_sign_array[idx] = true;
            idx++;
        }

        final_edge_list.push_back(edge_list[end_idx]);
        addition_sign_array[idx] = false;
        idx++;
        start_idx = end_idx + 1;
    }

    std::ofstream outputfile;
    outputfile.open(output_file_path, std::ios::app);
    std::string addition = "+";

    std::cout << "Writing Edges : " << final_edge_list.size() << " to output file" << std::endl;

    for (ui i = 0; i < final_edge_list.size(); i++){

        if(addition_sign_array[i]){
            addition = "+";
        }else{
            addition = "-";
        }

        outputfile << final_edge_list[i].first << "  " <<  final_edge_list[i].second << "  " <<  addition << std::endl;

        if(i % 1000 == 0){
            outputfile.flush();
        }

    }

    outputfile.flush();
    outputfile.close();
}


// Creating Streaming Graph 
/*int main(int argc, char** argv){

    std::string input_graph_file = "/home/kars1/Parallel_computation/dataset/com-youtube.ungraph.txt";
    std::string output_graph_file = "com-youtube_stm_20d.ungraph.txt";

    ui deletion_percentage = 20;
    create_streaming_graph_from_file(input_graph_file, output_graph_file, deletion_percentage);
}*/