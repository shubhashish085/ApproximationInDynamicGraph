#include "DistributedSampling.hpp"
#include <limits>
#include <random>
#include <cmath>


void ComputingNode::processAddition(Edge& edge){
    processEdgeForWorker(edge);
}

void ComputingNode::processDeletion(Edge& edge){
    processEdgeForWorker(edge);
}

void ComputingNode::processEdgeForWorker(Edge& edge){

    VertexID src = edge.src;
    VertexID dst = edge.dst;

    if (src == dst){
        return;
    }

    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<int> dis(0, s);
    std::mt19937_64 compensation_eng(rd());
    std::uniform_int_distribution<int> compensation_dis(0, nb + ng);
    
    double random_ratio = dis(eng);
    double random_compensation = compensation_dis(compensation_eng);

    if (src > dst)
    {
        VertexID temp = src;
        src = dst;
        dst = temp;
    }

    count_triangles(edge);

    bool isSample = false;
    if (edge.add)
    {
        // sample edge start
        if (ng + nb == 0)
        {
            if (edgeToIndex.size() < k)
            {
                addEdge(edge);
                
            }else if (random_ratio < k)
            {
                std::mt19937_64 key_selection_eng(rd());
                std::uniform_int_distribution<ui> uniform_key_dis(0, edgeToIndex.size() - 1);
                ui index = uniform_key_dis(key_selection_eng);
                Edge deletedEdge;
                deletedEdge.src = samples[0][index];
                deletedEdge.dst = samples[1][index];
                deleteEdge(deletedEdge); // remove a random edge from the samples
                addEdge(edge);                                // store the sampled edge
                
            }
        }
        else if (random_compensation < nb){
            addEdge(edge); // store the sampled edge
            nb--;
            
        }
        else{
            ng--;
        }
    }
    else
    {
        KeyID key = ((KeyID)src * std::numeric_limits<unsigned int>::max()) + dst;
        if (edgeToIndex.find(key) != edgeToIndex.end()){
            deleteEdge(edge); // remove the edge from the samples
            nb++;
            
        }else{
            ng++;
        }
    }

    if (edge.add){
        s++;
    }else{
        s--;
    }
    
    return;
}


void ComputingNode::processEdgeSquareForWorker(Edge& edge)
{

    VertexID src = edge.src;
    VertexID dst = edge.dst;

    if (src == dst){
        return;
    }

    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<int> dis(0, s);
    std::mt19937_64 compensation_eng(rd());
    std::uniform_int_distribution<int> compensation_dis(0, nb + ng);
    
    double random_ratio = dis(eng);
    double random_compensation = compensation_dis(compensation_eng);

    if (src > dst)
    {
        VertexID temp = src;
        src = dst;
        dst = temp;
    }

    count_squares(edge);

    bool isSample = false;
    if (edge.add)
    {
        // sample edge start
        if (ng + nb == 0)
        {
            if (edgeToIndex.size() < k)
            {
                addEdge(edge);
                
            }else if (random_ratio < k)
            {
                std::mt19937_64 key_selection_eng(rd());
                std::uniform_int_distribution<ui> uniform_key_dis(0, edgeToIndex.size() - 1);
                ui index = uniform_key_dis(key_selection_eng);
                Edge deletedEdge;
                deletedEdge.src = samples[0][index];
                deletedEdge.dst = samples[1][index];
                deleteEdge(deletedEdge); // remove a random edge from the samples
                addEdge(edge);                                // store the sampled edge
                
            }
        }
        else if (random_compensation < nb){
            addEdge(edge); // store the sampled edge
            nb--;
            
        }
        else{
            ng--;
        }
    }
    else
    {
        KeyID key = ((KeyID)src * std::numeric_limits<unsigned int>::max()) + dst;
        if (edgeToIndex.find(key) != edgeToIndex.end()){
            deleteEdge(edge); // remove the edge from the samples
            nb++;
            
        }else{
            ng++;
        }
    }

    if (edge.add){
        s++;
    }else{
        s--;
    }
    
    return;
}


void ComputingNode::addEdge(Edge& edge)
{
    VertexID src = edge.src;
    VertexID dst = edge.dst;
    ui sampleNum = edgeToIndex.size();
    samples[0][sampleNum] = src;
    samples[1][sampleNum] = dst;
    KeyID key = ((KeyID)src * std::numeric_limits<unsigned int>::max()) + dst;

    edgeToIndex.emplace(key, sampleNum);
    if (srcToDsts.find(src) == srcToDsts.end())
    {
        std::unordered_set<VertexID> dst_set;
        srcToDsts.emplace(src, dst_set);
    }
    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator itr = srcToDsts.find(src);
    (itr->second).insert(dst);

    if (srcToDsts.find(dst) == srcToDsts.end())
    {
        std::unordered_set<VertexID> src_set;
        srcToDsts.emplace(dst, src_set);
    }
    itr = srcToDsts.find(dst);
    (itr->second).insert(src);
}

void ComputingNode::deleteEdge(Edge& edge)
{
    VertexID src = edge.src;
    VertexID dst = edge.dst;
    ui sampleNum = edgeToIndex.size(), index;
    KeyID key = ((KeyID)src * std::numeric_limits<unsigned int>::max()) + dst;
    std::unordered_map<KeyID, ui>::iterator edge_idx_itr = edgeToIndex.find(key);
    if (edge_idx_itr != edgeToIndex.end())
    {
        index = edge_idx_itr->second;
        edgeToIndex.erase(key);
    }

    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator itr = srcToDsts.find(src);
    if (itr != srcToDsts.end())
    {
        (itr->second).erase(dst);
        if ((itr->second).empty())
        {
            srcToDsts.erase(src);
        }
    }

    itr = srcToDsts.find(dst);
    if (itr != srcToDsts.end())
    {
        (itr->second).erase(src);
        if ((itr->second).empty())
        {
            srcToDsts.erase(dst);
        }
    }

    if (index < sampleNum - 1)
    {
        int newSrc = samples[0][index] = samples[0][sampleNum - 1];
        int newDst = samples[1][index] = samples[1][sampleNum - 1];
        KeyID newKey = ((KeyID)newSrc * std::numeric_limits<unsigned int>::max()) + newDst;
        edgeToIndex.erase(newKey);
        edgeToIndex.emplace(newKey, index);
    }
}

double ComputingNode::getGlobalCnt(){
    return globalCnt;
}

std::unordered_map<VertexID, float> & ComputingNode::getLocalCnt(){
    return nodeToMotifs;
}



void ComputingNode::count_triangles(Edge& edge)
{
    VertexID src = edge.src;
    VertexID dst = edge.dst;

    // if this edge has a new node, there cannot be any triangles
    if (srcToDsts.find(src) == srcToDsts.end() || srcToDsts.find(dst) == srcToDsts.end()){
        return;
    }

    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator src_itr = srcToDsts.find(src);
    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator dst_itr = srcToDsts.find(dst);

    if ((src_itr->second).size() > (dst_itr->second).size())
    {
        std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator temp = src_itr;
        src_itr = dst_itr;
        dst_itr = temp;
    }

    double y = std::min((double)( k * 1.0), (double)(s + nb + ng));
    double weight = (s + nb + ng + 0.0) / y * (s + nb + ng - 1.0) / (y - 1.0);

    if (edge.add)
    {
        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, float>::iterator map_itr;


        while (itr != (src_itr->second).end())
        {
            VertexID neighbor = *itr;


            if (dst_set.find(neighbor) != dst_set.end()){
                count += 1;
                map_itr = nodeToMotifs.find(neighbor);
                if(map_itr == nodeToMotifs.end()){
                    nodeToMotifs[neighbor] = weight;
                }else{
                    nodeToMotifs[neighbor] = nodeToMotifs[neighbor] + weight; 
                }
            }

            itr++;
        }

        if (count > 0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToMotifs.find(src);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[src] = weight_sum;
            }else{
                nodeToMotifs[src] = nodeToMotifs[src] + weight_sum; 
            }

            map_itr = nodeToMotifs.find(dst);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[dst] = weight_sum;
            }else{
                nodeToMotifs[dst] = nodeToMotifs[dst] + weight_sum; 
            }

            globalCnt += weight_sum;
        }
    }
    else if (lowerBound)
    { // process the deletion with lower bounding

        double count = 0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, float>::iterator map_itr;

        while (itr != (src_itr->second).end())
        {
            VertexID neighbor = *itr;

            if (dst_set.find(neighbor) != dst_set.end()){
                count += 1;
                map_itr = nodeToMotifs.find(neighbor);
                if(map_itr == nodeToMotifs.end()){
                    nodeToMotifs[neighbor] = -weight;
                }else{
                    nodeToMotifs[neighbor] = nodeToMotifs[neighbor] - weight; 
                }
            }

            if(nodeToMotifs[neighbor] < 0){
                nodeToMotifs[neighbor] = 0;
            }

            itr++;
        }


        if (count > 0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToMotifs.find(src);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[src] = -weight_sum;
            }else{
                nodeToMotifs[src] = nodeToMotifs[src] - weight_sum; 
            }

            if (nodeToMotifs[src] < 0)
            {
                nodeToMotifs[src] = 0; // lower bounding
            }


            map_itr = nodeToMotifs.find(dst);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[dst] = -weight_sum;
            }else{
                nodeToMotifs[dst] = nodeToMotifs[dst] - weight_sum; 
            }

            if (nodeToMotifs[dst] < 0)
            {
                nodeToMotifs[dst] = 0; // lower bounding
            }

            globalCnt -= weight_sum;            
            globalCnt = std::max(0.0, globalCnt); // lower bounding
        }
    }

    else
    { // process the deletion without lower bounding

        double count = 0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, float>::iterator map_itr;

        while (itr != (src_itr->second).end())
        {
            VertexID neighbor = *itr;

            if (dst_set.find(neighbor) != dst_set.end()){
                count += 1;
                map_itr = nodeToMotifs.find(neighbor);
                if(map_itr == nodeToMotifs.end()){
                    nodeToMotifs[neighbor] = -weight;
                }else{
                    nodeToMotifs[neighbor] = nodeToMotifs[neighbor] - weight; 
                }
            }

            if(nodeToMotifs[neighbor] < 0){
                nodeToMotifs[neighbor] = 0;
            }

            itr++;
        }

        if (count > 0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToMotifs.find(src);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[src] = -weight_sum;
            }else{
                nodeToMotifs[src] = nodeToMotifs[src] - weight_sum; 
            }

            map_itr = nodeToMotifs.find(dst);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[dst] = -weight_sum;
            }else{
                nodeToMotifs[dst] = nodeToMotifs[dst] - weight_sum; 
            }

            globalCnt -= weight_sum;
        }
    }
}


void ComputingNode::count_squares(Edge& edge)
{

    VertexID src = edge.src;
    VertexID dst = edge.dst;

    // if this edge has a new node, there cannot be any triangles
    if (srcToDsts.find(src) == srcToDsts.end() || srcToDsts.find(dst) == srcToDsts.end()){
        return;
    }

    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator src_itr = srcToDsts.find(src);
    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator dst_itr = srcToDsts.find(dst);

    if ((src_itr->second).size() > (dst_itr->second).size())
    {
        std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator temp = src_itr;
        src_itr = dst_itr;
        dst_itr = temp;
    }

    double y = std::min((double)( k * 1.0), (double)(s + nb + ng));
    double weight = (s + nb + ng + 0.0) / y * (s + nb + ng - 1.0) / (y - 1.0) * (s + nb + ng - 2.0) / (y - 2.0); 

    VertexID neighbor, two_hop_neighbor;

    if (edge.add)
    {
        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_set<VertexID> src_set = src_itr->second;
        dst_set.erase(src);
        src_set.erase(dst);

        std::unordered_map<VertexID, float>::iterator map_itr;
        std::unordered_map<VertexID, float>::iterator two_hop_map_itr;
        std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator two_hop_itr;
        std::unordered_set<VertexID>::iterator two_hop_nbr_itr;


        while (itr != (src_itr->second).end())
        {
            neighbor = *itr;
            two_hop_itr = srcToDsts.find(neighbor);

            if(neighbor != dst){

                for(two_hop_nbr_itr = (two_hop_itr->second).begin(); two_hop_nbr_itr != (two_hop_itr->second).end(); two_hop_nbr_itr++){
                    two_hop_neighbor = *two_hop_nbr_itr;

                    if (dst_set.find(two_hop_neighbor) != dst_set.end() && two_hop_neighbor != src){
                        count += 1.0;
                        map_itr = nodeToMotifs.find(neighbor);
                        if(map_itr == nodeToMotifs.end()){
                            nodeToMotifs[neighbor] = weight;
                        }else{
                            nodeToMotifs[neighbor] = nodeToMotifs[neighbor] + weight; 
                        }

                        two_hop_map_itr = nodeToMotifs.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToMotifs.end()){
                            nodeToMotifs[two_hop_neighbor] = weight;
                        }else{
                            nodeToMotifs[two_hop_neighbor] = nodeToMotifs[two_hop_neighbor] + weight; 
                        }
                    }
                }

            }

            itr++;
        }

        if (count > 0.0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToMotifs.find(src);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[src] = weight_sum;
            }else{
                nodeToMotifs[src] = nodeToMotifs[src] + weight_sum; 
            }

            map_itr = nodeToMotifs.find(dst);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[dst] = weight_sum;
            }else{
                nodeToMotifs[dst] = nodeToMotifs[dst] + weight_sum; 
            }

            globalCnt += weight_sum;
        }
    }
    else if (lowerBound)
    { // process the deletion with lower bounding

        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_set<VertexID> src_set = src_itr->second;
        dst_set.erase(src);
        src_set.erase(dst);
        std::unordered_map<VertexID, float>::iterator map_itr;
        std::unordered_map<VertexID, float>::iterator two_hop_map_itr;
        std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator two_hop_itr;
        std::unordered_set<VertexID>::iterator two_hop_nbr_itr;

        while (itr != (src_itr->second).end())
        {
            neighbor = *itr;
            two_hop_itr = srcToDsts.find(neighbor);

            if(neighbor != dst){

                for(two_hop_nbr_itr = (two_hop_itr->second).begin(); two_hop_nbr_itr != (two_hop_itr->second).end(); two_hop_nbr_itr++){
                    two_hop_neighbor = *two_hop_nbr_itr;

                    if (dst_set.find(two_hop_neighbor) != dst_set.end() && two_hop_neighbor != src){
                        count += 1;
                        map_itr = nodeToMotifs.find(neighbor);
                        if(map_itr == nodeToMotifs.end()){
                            nodeToMotifs[neighbor] = -weight;
                        }else{
                            nodeToMotifs[neighbor] = nodeToMotifs[neighbor] - weight; 
                        }

                        two_hop_map_itr = nodeToMotifs.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToMotifs.end()){
                            nodeToMotifs[two_hop_neighbor] = -weight;
                        }else{
                            nodeToMotifs[two_hop_neighbor] = nodeToMotifs[two_hop_neighbor] - weight; 
                        }
                    }

                    if(nodeToMotifs[two_hop_neighbor] < 0){
                        nodeToMotifs[two_hop_neighbor] = 0;
                    }
                }

                if(nodeToMotifs[neighbor] < 0){
                    nodeToMotifs[neighbor] = 0;
                }
            }

            itr++;
        }


        if (count > 0.0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToMotifs.find(src);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[src] = -weight_sum;
            }else{
                nodeToMotifs[src] = nodeToMotifs[src] - weight_sum; 
            }

            if (nodeToMotifs[src] < 0)
            {
                nodeToMotifs[src] = 0; // lower bounding
            }


            map_itr = nodeToMotifs.find(dst);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[dst] = -weight_sum;
            }else{
                nodeToMotifs[dst] = nodeToMotifs[dst] - weight_sum; 
            }

            if (nodeToMotifs[dst] < 0)
            {
                nodeToMotifs[dst] = 0; // lower bounding
            }

            globalCnt -= weight_sum;            
            globalCnt = std::max(0.0, globalCnt); // lower bounding
        }
    }

    else
    { // process the deletion without lower bounding

        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_set<VertexID> src_set = src_itr->second;
        dst_set.erase(src);
        src_set.erase(dst);
        std::unordered_map<VertexID, float>::iterator map_itr;
        std::unordered_map<VertexID, float>::iterator two_hop_map_itr;
        std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator two_hop_itr;
        std::unordered_set<VertexID>::iterator two_hop_nbr_itr;

        while (itr != (src_itr->second).end())
        {
            neighbor = *itr;
            two_hop_itr = srcToDsts.find(neighbor);

            if(neighbor != dst){

                for(two_hop_nbr_itr = (two_hop_itr->second).begin(); two_hop_nbr_itr != (two_hop_itr->second).end(); two_hop_nbr_itr++){
                    two_hop_neighbor = *two_hop_nbr_itr;

                    if (dst_set.find(two_hop_neighbor) != dst_set.end() && two_hop_neighbor != src){
                        count += 1;
                        map_itr = nodeToMotifs.find(neighbor);
                        if(map_itr == nodeToMotifs.end()){
                            nodeToMotifs[neighbor] = -weight;
                        }else{
                            nodeToMotifs[neighbor] = nodeToMotifs[neighbor] - weight; 
                        }

                        two_hop_map_itr = nodeToMotifs.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToMotifs.end()){
                            nodeToMotifs[two_hop_neighbor] = -weight;
                        }else{
                            nodeToMotifs[two_hop_neighbor] = nodeToMotifs[two_hop_neighbor] - weight; 
                        }
                    }

                    if(nodeToMotifs[two_hop_neighbor] < 0){
                        nodeToMotifs[two_hop_neighbor] = 0;
                    }
                }

                if(nodeToMotifs[neighbor] < 0){
                    nodeToMotifs[neighbor] = 0;
                }
            }

            itr++;
        }

        if (count > 0.0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToMotifs.find(src);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[src] = -weight_sum;
            }else{
                nodeToMotifs[src] = nodeToMotifs[src] - weight_sum; 
            }

            map_itr = nodeToMotifs.find(dst);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[dst] = -weight_sum;
            }else{
                nodeToMotifs[dst] = nodeToMotifs[dst] - weight_sum; 
            }

            globalCnt -= weight_sum;
        }
    }
}






