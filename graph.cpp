#include <fstream>
#include <sstream>
#include "graph.h"


void Graph::loadGraphFromFile(const std::string& file_path){

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

        if (input_line.rfind("#", 0) == 0) {

            line_count++;

            if (input_line.rfind("# Nodes", 0) == 0) {
                std::stringstream ss(input_line);
                std::string token;
                int count = 0;
                while (!ss.eof()) {
                    std::getline(ss, token, ' ');
                    if (!(token.rfind("#", 0) == 0 || token.rfind("Nodes:", 0) == 0 || token.rfind("Edges:", 0) == 0)) {
                        if (count == 0) {
                            vertices_count = stoi(token);
                            degrees = new ui[vertices_count];
                            std::fill(degrees, degrees + vertices_count, 0);
                            count = 1;
                        } else {
                            edges_count = stoi(token);
                            count = 0;
                        }
                        std::cout << "Vertices Count : " << vertices_count << " Edges Count : " << edges_count << std::endl;
                    }
                }
            }
        }

        if(line_count >= comment_line_count){
            break;
        }
    }

    VertexID begin, end;


    while(infile >> begin) {

        infile >> end;

        if (begin != end && begin < vertices_count && end < vertices_count) {
            degrees[begin] += 1;
            degrees[end] += 1;
        }
    }

    infile.close();

    std::ifstream input_file(file_path);

    offsets = new ui[vertices_count +  1];
    offsets[0] = 0;

    neighbors = new VertexID[edges_count];
    max_degree = 0;

    std::cout << "Initialization Finished" << std::endl;

    LabelID max_label_id = 0, begin_vtx_label, end_vtx_label;
    std::vector<ui> neighbors_offset(vertices_count, 0);// used for adjust neighbors with offset

    for(ui id = 0; id < vertices_count; id++){
        offsets[id + 1] = offsets[id] + degrees[id];

        if (degrees[id] > max_degree) {
            max_degree = degrees[id];
        }
    }

    line_count = 0;


    while(input_file >> begin){ // Read edge.

        input_file >> end;

        line_count++;
        if(begin >= vertices_count || end >= vertices_count || begin == end){
            continue;
        }

        ui offset = offsets[begin] + neighbors_offset[begin];
        neighbors[offset] = end;

        offset = offsets[end] + neighbors_offset[end];
        neighbors[offset] = begin;

        neighbors_offset[begin] += 1;
        neighbors_offset[end] += 1;

    }

    input_file.close();
}


long long Graph::count_exact_triangle(){

    long long exact_count = 0;
    ui set_intersection_length = 0;

    for(auto entry:adj_list){

        for(auto set_itr = entry.second.begin(); set_itr != entry.second.end(); set_itr++){
            set_intersection_length = get_nbr_set_intersection_count(entry.first, *set_itr);
            exact_count += set_intersection_length;    
        }
    }

    return (exact_count/3);

}

long long Graph::count_exact_square(){

    long long exact_count = 0;
    ui set_intersection_length = 0;
    std::set<VertexID> u_nbrs, v_nbrs, y_nbrs;
    std::set<VertexID> intersection_set;

    for(auto entry:adj_list){
        u_nbrs = entry.second;
        for(auto set_itr = entry.second.begin(); set_itr != entry.second.end(); set_itr++){
            v_nbrs = adj_list[*set_itr];
            for(auto u_nbr_itr = u_nbrs.begin(); u_nbr_itr != u_nbrs.end(); u_nbr_itr++){
                if(*u_nbr_itr != *set_itr){
                    y_nbrs = adj_list[*u_nbr_itr];
                    std::set_intersection(v_nbrs.begin(), v_nbrs.end(), adj_list[*u_nbr_itr].begin(), adj_list[*u_nbr_itr].end(), std::inserter(intersection_set, intersection_set.begin()));
                    set_intersection_length = intersection_set.size() - 1;
                    exact_count += set_intersection_length; 
                }
            }
        }
    }

    return (exact_count/4);

}


long long Graph::count_exact_butterfly(){

    long long exact_count = 0;
    ui set_intersection_length = 0;
    std::set<VertexID> u_nbrs, v_nbrs, y_nbrs;
    std::set<VertexID> intersection_set;

    for(auto entry:adj_list){
        u_nbrs = entry.second;
        for(auto set_itr = entry.second.begin(); set_itr != entry.second.end(); set_itr++){
            v_nbrs = adj_list[*set_itr];
            for(auto u_nbr_itr = u_nbrs.begin(); u_nbr_itr != u_nbrs.end(); u_nbr_itr++){
                if(*u_nbr_itr != *set_itr){
                    y_nbrs = adj_list[*u_nbr_itr];
                    std::set_intersection(v_nbrs.begin(), v_nbrs.end(), adj_list[*u_nbr_itr].begin(), adj_list[*u_nbr_itr].end(), std::inserter(intersection_set, intersection_set.begin()));
                    set_intersection_length = intersection_set.size() - 1;
                    exact_count += set_intersection_length; 
                }
            }
        }
    }

    return (exact_count/4);

}