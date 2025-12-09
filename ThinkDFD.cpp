#include "ThinkDFD.hpp"
#include <limits>
#include <random>


void ThinkDFD::processAddition(VertexID src, VertexID dst){
    processEdge(src, dst, true);
}

void ThinkDFD::processDeletion(VertexID src, VertexID dst){
    processEdge(src, dst, false);
}

void ThinkDFD::processEdge(VertexID src, VertexID dst, bool add)
{

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

    count_triangles(src, dst, add);

    bool isSample = false;
    if (add)
    {
        // sample edge start
        if (ng + nb == 0)
        {
            if (edgeToIndex.size() < k)
            {
                addEdge(src, dst);
                
            }else if (random_ratio < k)
            {
                std::mt19937_64 key_selection_eng(rd());
                std::uniform_int_distribution<ui> uniform_key_dis(0, edgeToIndex.size() - 1);
                ui index = uniform_key_dis(key_selection_eng);
                deleteEdge(samples[0][index], samples[1][index]); // remove a random edge from the samples
                addEdge(src, dst);                                // store the sampled edge
                
            }
        }
        else if (random_compensation < nb){
            addEdge(src, dst); // store the sampled edge
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
            deleteEdge(src, dst); // remove the edge from the samples
            nb++;
            
        }else{
            ng++;
        }
    }

    if (add){
        s++;
    }else{
        s--;
    }
    
         // count the added or deleted triangles
    
    return;
}


void ThinkDFD::processEdgeSquare(VertexID src, VertexID dst, bool add)
{

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

    count_squares(src, dst, add);

    bool isSample = false;
    if (add)
    {
        // sample edge start
        if (ng + nb == 0)
        {
            if (edgeToIndex.size() < k)
            {
                addEdge(src, dst);
                
            }else if (random_ratio < k)
            {
                std::mt19937_64 key_selection_eng(rd());
                std::uniform_int_distribution<ui> uniform_key_dis(0, edgeToIndex.size() - 1);
                ui index = uniform_key_dis(key_selection_eng);
                deleteEdge(samples[0][index], samples[1][index]); // remove a random edge from the samples
                addEdge(src, dst);                                // store the sampled edge
                
            }
        }
        else if (random_compensation < nb){
            addEdge(src, dst); // store the sampled edge
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
            deleteEdge(src, dst); // remove the edge from the samples
            nb++;
            
        }else{
            ng++;
        }
    }

    if (add){
        s++;
    }else{
        s--;
    }
    
         // count the added or deleted triangles
    
    return;
}


void ThinkDFD::processEdgeButterfly(VertexID src, VertexID dst, bool add)
{

    if (src == dst){
        return;
    }

    std::random_device rd;
    std::mt19937 eng(rd());
    
    std::mt19937 compensation_eng(rd());
    std::uniform_int_distribution<int> compensation_dis(0, nb + ng);
    
    int random_ratio;
    int random_compensation = compensation_dis(compensation_eng);

    if (src > dst)
    {
        VertexID temp = src;
        src = dst;
        dst = temp;
    }

    //count_butterfly(src, dst, add);
    count_butterfly_with_removal(src, dst, add);


    bool isSample = false;
    if (add)
    {
        s++;

        std::uniform_int_distribution<int> dis(0, s);
        random_ratio = dis(eng);

        // sample edge start
        if (ng + nb == 0)
        {
            if (edgeToIndex.size() < k)
            {
                addEdge(src, dst);
                
            }else if (random_ratio < k)
            {
                std::mt19937 key_selection_eng(rd());
                std::uniform_int_distribution<int> uniform_key_dis(0, edgeToIndex.size() - 1);
                ui index = uniform_key_dis(key_selection_eng);
                deleteEdge(samples[0][index], samples[1][index]); // remove a random edge from the samples
                addEdge(src, dst);                                // store the sampled edge
                
            }
        }else if (random_compensation < nb){
            addEdge(src, dst); // store the sampled edge
            nb--;
            
        }else{
            ng--;
        }
    }
    else
    {
        s--;
        KeyID key = ((KeyID)src * std::numeric_limits<unsigned int>::max()) + dst;
        if (edgeToIndex.find(key) != edgeToIndex.end()){
            deleteEdge(src, dst); // remove the edge from the samples
            nb++;
            
        }else{
            ng++;
        }
    }

    return;
}



void ThinkDFD::addEdge(VertexID src, VertexID dst)
{

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

void ThinkDFD::deleteEdge(VertexID src, VertexID dst)
{

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

double ThinkDFD::getGlobalTriangle(){
    return globalTriangle;
}

double ThinkDFD::getGlobalSquare(){
    return globalSquare;
}

double ThinkDFD::getGlobalButterfly(){
    return globalButterfly;
}

std::unordered_map<VertexID, double> ThinkDFD::getLocalTriangle(){
    return nodeToTriangles;
}

std::unordered_map<VertexID, double> ThinkDFD::getLocalSquare(){
    return nodeToSquares;
}

void ThinkDFD::count_triangles(VertexID src, VertexID dst, bool add)
{

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

    if (add)
    {
        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, double>::iterator map_itr;


        while (itr != (src_itr->second).end())
        {
            VertexID neighbor = *itr;


            if (dst_set.find(neighbor) != dst_set.end()){
                count += 1;
                map_itr = nodeToTriangles.find(neighbor);
                if(map_itr == nodeToTriangles.end()){
                    nodeToTriangles[neighbor] = weight;
                }else{
                    nodeToTriangles[neighbor] = nodeToTriangles[neighbor] + weight; 
                }
            }

            itr++;
        }

        if (count > 0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToTriangles.find(src);
            if(map_itr == nodeToTriangles.end()){
                nodeToTriangles[src] = weight_sum;
            }else{
                nodeToTriangles[src] = nodeToTriangles[src] + weight_sum; 
            }

            map_itr = nodeToTriangles.find(dst);
            if(map_itr == nodeToTriangles.end()){
                nodeToTriangles[dst] = weight_sum;
            }else{
                nodeToTriangles[dst] = nodeToTriangles[dst] + weight_sum; 
            }

            globalTriangle += weight_sum;
        }
    }
    else if (lowerBound)
    { // process the deletion with lower bounding

        double count = 0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, double>::iterator map_itr;

        while (itr != (src_itr->second).end())
        {
            VertexID neighbor = *itr;

            if (dst_set.find(neighbor) != dst_set.end()){
                count += 1;
                map_itr = nodeToTriangles.find(neighbor);
                if(map_itr == nodeToTriangles.end()){
                    nodeToTriangles[neighbor] = -weight;
                }else{
                    nodeToTriangles[neighbor] = nodeToTriangles[neighbor] - weight; 
                }
            }

            if(nodeToTriangles[neighbor] < 0){
                nodeToTriangles[neighbor] = 0;
            }

            itr++;
        }


        if (count > 0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToTriangles.find(src);
            if(map_itr == nodeToTriangles.end()){
                nodeToTriangles[src] = -weight_sum;
            }else{
                nodeToTriangles[src] = nodeToTriangles[src] - weight_sum; 
            }

            if (nodeToTriangles[src] < 0)
            {
                nodeToTriangles[src] = 0; // lower bounding
            }


            map_itr = nodeToTriangles.find(dst);
            if(map_itr == nodeToTriangles.end()){
                nodeToTriangles[dst] = -weight_sum;
            }else{
                nodeToTriangles[dst] = nodeToTriangles[dst] - weight_sum; 
            }

            if (nodeToTriangles[dst] < 0)
            {
                nodeToTriangles[dst] = 0; // lower bounding
            }

            globalTriangle -= weight_sum;            
            globalTriangle = std::max(0.0, globalTriangle); // lower bounding
        }
    }

    else
    { // process the deletion without lower bounding

        double count = 0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, double>::iterator map_itr;

        while (itr != (src_itr->second).end())
        {
            VertexID neighbor = *itr;

            if (dst_set.find(neighbor) != dst_set.end()){
                count += 1;
                map_itr = nodeToTriangles.find(neighbor);
                if(map_itr == nodeToTriangles.end()){
                    nodeToTriangles[neighbor] = -weight;
                }else{
                    nodeToTriangles[neighbor] = nodeToTriangles[neighbor] - weight; 
                }
            }

            if(nodeToTriangles[neighbor] < 0){
                nodeToTriangles[neighbor] = 0;
            }

            itr++;
        }

        if (count > 0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToTriangles.find(src);
            if(map_itr == nodeToTriangles.end()){
                nodeToTriangles[src] = -weight_sum;
            }else{
                nodeToTriangles[src] = nodeToTriangles[src] - weight_sum; 
            }

            map_itr = nodeToTriangles.find(dst);
            if(map_itr == nodeToTriangles.end()){
                nodeToTriangles[dst] = -weight_sum;
            }else{
                nodeToTriangles[dst] = nodeToTriangles[dst] - weight_sum; 
            }

            globalTriangle -= weight_sum;
        }
    }
}


void ThinkDFD::count_squares(VertexID src, VertexID dst, bool add)
{

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

    if (add)
    {
        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, double>::iterator map_itr;
        std::unordered_map<VertexID, double>::iterator two_hop_map_itr;
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
                        map_itr = nodeToSquares.find(neighbor);
                        if(map_itr == nodeToSquares.end()){
                            nodeToSquares[neighbor] = weight;
                        }else{
                            nodeToSquares[neighbor] = nodeToSquares[neighbor] + weight; 
                        }

                        two_hop_map_itr = nodeToSquares.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToSquares.end()){
                            nodeToSquares[two_hop_neighbor] = weight;
                        }else{
                            nodeToSquares[two_hop_neighbor] = nodeToSquares[two_hop_neighbor] + weight; 
                        }
                    }
                }

            }

            itr++;
        }

        if (count > 0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToSquares.find(src);
            if(map_itr == nodeToSquares.end()){
                nodeToSquares[src] = weight_sum;
            }else{
                nodeToSquares[src] = nodeToSquares[src] + weight_sum; 
            }

            map_itr = nodeToSquares.find(dst);
            if(map_itr == nodeToSquares.end()){
                nodeToSquares[dst] = weight_sum;
            }else{
                nodeToSquares[dst] = nodeToSquares[dst] + weight_sum; 
            }

            globalSquare += weight_sum;
        }
    }
    else if (lowerBound)
    { // process the deletion with lower bounding

        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, double>::iterator map_itr;
        std::unordered_map<VertexID, double>::iterator two_hop_map_itr;
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
                        map_itr = nodeToSquares.find(neighbor);
                        if(map_itr == nodeToSquares.end()){
                            nodeToSquares[neighbor] = -weight;
                        }else{
                            nodeToSquares[neighbor] = nodeToSquares[neighbor] - weight; 
                        }

                        two_hop_map_itr = nodeToSquares.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToSquares.end()){
                            nodeToSquares[two_hop_neighbor] = -weight;
                        }else{
                            nodeToSquares[two_hop_neighbor] = nodeToSquares[two_hop_neighbor] - weight; 
                        }
                    }

                    if(nodeToSquares[two_hop_neighbor] < 0){
                        nodeToSquares[two_hop_neighbor] = 0;
                    }
                }

                if(nodeToSquares[neighbor] < 0){
                    nodeToSquares[neighbor] = 0;
                }
            }

            itr++;
        }


        if (count > 0.0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToSquares.find(src);
            if(map_itr == nodeToSquares.end()){
                nodeToSquares[src] = -weight_sum;
            }else{
                nodeToSquares[src] = nodeToSquares[src] - weight_sum; 
            }

            if (nodeToSquares[src] < 0)
            {
                nodeToSquares[src] = 0; // lower bounding
            }


            map_itr = nodeToSquares.find(dst);
            if(map_itr == nodeToSquares.end()){
                nodeToSquares[dst] = -weight_sum;
            }else{
                nodeToSquares[dst] = nodeToSquares[dst] - weight_sum; 
            }

            if (nodeToSquares[dst] < 0)
            {
                nodeToSquares[dst] = 0; // lower bounding
            }

            globalSquare -= weight_sum;            
            globalSquare = std::max(0.0, globalSquare); // lower bounding
        }
    }

    else
    { // process the deletion without lower bounding

        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, double>::iterator map_itr;
        std::unordered_map<VertexID, double>::iterator two_hop_map_itr;
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
                        map_itr = nodeToSquares.find(neighbor);
                        if(map_itr == nodeToSquares.end()){
                            nodeToSquares[neighbor] = -weight;
                        }else{
                            nodeToSquares[neighbor] = nodeToSquares[neighbor] - weight; 
                        }

                        two_hop_map_itr = nodeToSquares.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToSquares.end()){
                            nodeToSquares[two_hop_neighbor] = -weight;
                        }else{
                            nodeToSquares[two_hop_neighbor] = nodeToSquares[two_hop_neighbor] - weight; 
                        }
                    }

                    if(nodeToSquares[two_hop_neighbor] < 0){
                        nodeToSquares[two_hop_neighbor] = 0;
                    }
                }

                if(nodeToSquares[neighbor] < 0){
                    nodeToSquares[neighbor] = 0;
                }
            }

            itr++;
        }

        if (count > 0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToSquares.find(src);
            if(map_itr == nodeToSquares.end()){
                nodeToSquares[src] = -weight_sum;
            }else{
                nodeToSquares[src] = nodeToSquares[src] - weight_sum; 
            }

            map_itr = nodeToSquares.find(dst);
            if(map_itr == nodeToSquares.end()){
                nodeToSquares[dst] = -weight_sum;
            }else{
                nodeToSquares[dst] = nodeToSquares[dst] - weight_sum; 
            }

            globalSquare -= weight_sum;
        }
    }
}


void ThinkDFD::count_butterfly(VertexID src, VertexID dst, bool add)
{

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

    //double y = std::min((double)( k * 1.0), (double)(s + nb + ng));
    //double weight = std::max((s + nb + ng + 0.0) / (k * 1.0) * (s + nb + ng - 1.0) / (k - 1.0) * (s + nb + ng - 2.0) / (k - 2.0), 1.0); 

    VertexID neighbor, two_hop_neighbor;

    if (add)
    {
        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, double>::iterator map_itr;
        std::unordered_map<VertexID, double>::iterator two_hop_map_itr;
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
                        map_itr = nodeToButterflies.find(neighbor);
                        if(map_itr == nodeToButterflies.end()){
                            nodeToButterflies[neighbor] = weight;
                        }else{
                            nodeToButterflies[neighbor] = nodeToButterflies[neighbor] + weight; 
                        }

                        two_hop_map_itr = nodeToButterflies.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToButterflies.end()){
                            nodeToButterflies[two_hop_neighbor] = weight;
                        }else{
                            nodeToButterflies[two_hop_neighbor] = nodeToButterflies[two_hop_neighbor] + weight; 
                        }
                    }
                }
            }

            itr++;
        }

        if (count > 0.0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToButterflies.find(src);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[src] = weight_sum;
            }else{
                nodeToButterflies[src] = nodeToButterflies[src] + weight_sum; 
            }

            map_itr = nodeToButterflies.find(dst);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[dst] = weight_sum;
            }else{
                nodeToButterflies[dst] = nodeToButterflies[dst] + weight_sum; 
            }

            globalButterfly += weight_sum;
        }
    }
    else if (lowerBound)
    { // process the deletion with lower bounding

        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, double>::iterator map_itr;
        std::unordered_map<VertexID, double>::iterator two_hop_map_itr;
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
                        map_itr = nodeToButterflies.find(neighbor);
                        if(map_itr == nodeToButterflies.end()){
                            nodeToButterflies[neighbor] = -weight;
                        }else{
                            nodeToButterflies[neighbor] = nodeToButterflies[neighbor] - weight; 
                        }

                        two_hop_map_itr = nodeToButterflies.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToButterflies.end()){
                            nodeToButterflies[two_hop_neighbor] = -weight;
                        }else{
                            nodeToButterflies[two_hop_neighbor] = nodeToButterflies[two_hop_neighbor] - weight; 
                        }
                    }

                    if(nodeToButterflies[two_hop_neighbor] < 0){
                        nodeToButterflies[two_hop_neighbor] = 0;
                    }
                }

                if(nodeToButterflies[neighbor] < 0){
                    nodeToButterflies[neighbor] = 0;
                }
            }

            itr++;
        }


        if (count > 0.0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToButterflies.find(src);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[src] = -weight_sum;
            }else{
                nodeToButterflies[src] = nodeToButterflies[src] - weight_sum; 
            }

            if (nodeToButterflies[src] < 0)
            {
                nodeToButterflies[src] = 0; // lower bounding
            }


            map_itr = nodeToButterflies.find(dst);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[dst] = -weight_sum;
            }else{
                nodeToButterflies[dst] = nodeToButterflies[dst] - weight_sum; 
            }

            if (nodeToButterflies[dst] < 0)
            {
                nodeToButterflies[dst] = 0; // lower bounding
            }

            globalButterfly -= weight_sum;            
            globalButterfly = std::max(0.0, globalButterfly); // lower bounding
        }
    }

    else
    { // process the deletion without lower bounding

        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_map<VertexID, double>::iterator map_itr;
        std::unordered_map<VertexID, double>::iterator two_hop_map_itr;
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
                        map_itr = nodeToButterflies.find(neighbor);
                        if(map_itr == nodeToButterflies.end()){
                            nodeToButterflies[neighbor] = -weight;
                        }else{
                            nodeToButterflies[neighbor] = nodeToButterflies[neighbor] - weight; 
                        }

                        two_hop_map_itr = nodeToButterflies.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToButterflies.end()){
                            nodeToButterflies[two_hop_neighbor] = -weight;
                        }else{
                            nodeToButterflies[two_hop_neighbor] = nodeToButterflies[two_hop_neighbor] - weight; 
                        }
                    }

                    if(nodeToButterflies[two_hop_neighbor] < 0){
                        nodeToButterflies[two_hop_neighbor] = 0;
                    }
                }

                if(nodeToButterflies[neighbor] < 0){
                    nodeToButterflies[neighbor] = 0;
                }
            }

            itr++;
        }

        if (count > 0.0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToButterflies.find(src);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[src] = -weight_sum;
            }else{
                nodeToButterflies[src] = nodeToButterflies[src] - weight_sum; 
            }

            map_itr = nodeToButterflies.find(dst);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[dst] = -weight_sum;
            }else{
                nodeToButterflies[dst] = nodeToButterflies[dst] - weight_sum; 
            }

            globalButterfly -= weight_sum;
        }
    }
}


void ThinkDFD::count_butterfly_with_removal(VertexID src, VertexID dst, bool add)
{

    // if this edge has a new node, there cannot be any triangles
    if (srcToDsts.find(src) == srcToDsts.end() || srcToDsts.find(dst) == srcToDsts.end()){
        return;
    }

    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator src_itr = srcToDsts.find(src);
    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator dst_itr = srcToDsts.find(dst);

    /*if ((src_itr->second).size() > (dst_itr->second).size())
    {
        std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator temp = src_itr;
        src_itr = dst_itr;
        dst_itr = temp;
    }*/

    double y = std::min((double)( k * 1.0), (double)(s + nb + ng));
    double weight = (s + nb + ng + 0.0) / y * (s + nb + ng - 1.0) / (y - 1.0) * (s + nb + ng - 2.0) / (y - 2.0); 

    //double y = std::min((double)( k * 1.0), (double)(s + nb + ng));
    //double weight = std::max((s + nb + ng + 0.0) / (k * 1.0) * (s + nb + ng - 1.0) / (k - 1.0) * (s + nb + ng - 2.0) / (k - 2.0), 1.0); 

    VertexID neighbor, two_hop_neighbor;

    if (add)
    {
        double count = 0.0;
        std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
        std::unordered_set<VertexID> dst_set = dst_itr->second;
        std::unordered_set<VertexID> src_set = src_itr->second;
        dst_set.erase(src);
        src_set.erase(dst);

        std::unordered_map<VertexID, double>::iterator map_itr;
        std::unordered_map<VertexID, double>::iterator two_hop_map_itr;
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
                        map_itr = nodeToButterflies.find(neighbor);
                        if(map_itr == nodeToButterflies.end()){
                            nodeToButterflies[neighbor] = weight;
                        }else{
                            nodeToButterflies[neighbor] = nodeToButterflies[neighbor] + weight; 
                        }

                        two_hop_map_itr = nodeToButterflies.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToButterflies.end()){
                            nodeToButterflies[two_hop_neighbor] = weight;
                        }else{
                            nodeToButterflies[two_hop_neighbor] = nodeToButterflies[two_hop_neighbor] + weight; 
                        }
                    }
                }
            }

            itr++;
        }

        if (count > 0.0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToButterflies.find(src);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[src] = weight_sum;
            }else{
                nodeToButterflies[src] = nodeToButterflies[src] + weight_sum; 
            }

            map_itr = nodeToButterflies.find(dst);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[dst] = weight_sum;
            }else{
                nodeToButterflies[dst] = nodeToButterflies[dst] + weight_sum; 
            }

            globalButterfly += weight_sum;
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

        std::unordered_map<VertexID, double>::iterator map_itr;
        std::unordered_map<VertexID, double>::iterator two_hop_map_itr;
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
                        map_itr = nodeToButterflies.find(neighbor);
                        if(map_itr == nodeToButterflies.end()){
                            nodeToButterflies[neighbor] = -weight;
                        }else{
                            nodeToButterflies[neighbor] = nodeToButterflies[neighbor] - weight; 
                        }

                        two_hop_map_itr = nodeToButterflies.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToButterflies.end()){
                            nodeToButterflies[two_hop_neighbor] = -weight;
                        }else{
                            nodeToButterflies[two_hop_neighbor] = nodeToButterflies[two_hop_neighbor] - weight; 
                        }
                    }

                    if(nodeToButterflies[two_hop_neighbor] < 0){
                        nodeToButterflies[two_hop_neighbor] = 0;
                    }
                }

                if(nodeToButterflies[neighbor] < 0){
                    nodeToButterflies[neighbor] = 0;
                }
            }

            itr++;
        }


        if (count > 0.0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToButterflies.find(src);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[src] = -weight_sum;
            }else{
                nodeToButterflies[src] = nodeToButterflies[src] - weight_sum; 
            }

            if (nodeToButterflies[src] < 0)
            {
                nodeToButterflies[src] = 0; // lower bounding
            }


            map_itr = nodeToButterflies.find(dst);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[dst] = -weight_sum;
            }else{
                nodeToButterflies[dst] = nodeToButterflies[dst] - weight_sum; 
            }

            if (nodeToButterflies[dst] < 0)
            {
                nodeToButterflies[dst] = 0; // lower bounding
            }

            globalButterfly -= weight_sum;            
            globalButterfly = std::max(0.0, globalButterfly); // lower bounding
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

        std::unordered_map<VertexID, double>::iterator map_itr;
        std::unordered_map<VertexID, double>::iterator two_hop_map_itr;
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
                        map_itr = nodeToButterflies.find(neighbor);
                        if(map_itr == nodeToButterflies.end()){
                            nodeToButterflies[neighbor] = -weight;
                        }else{
                            nodeToButterflies[neighbor] = nodeToButterflies[neighbor] - weight; 
                        }

                        two_hop_map_itr = nodeToButterflies.find(two_hop_neighbor);
                        if(two_hop_map_itr == nodeToButterflies.end()){
                            nodeToButterflies[two_hop_neighbor] = -weight;
                        }else{
                            nodeToButterflies[two_hop_neighbor] = nodeToButterflies[two_hop_neighbor] - weight; 
                        }
                    }

                    if(nodeToButterflies[two_hop_neighbor] < 0){
                        nodeToButterflies[two_hop_neighbor] = 0;
                    }
                }

                if(nodeToButterflies[neighbor] < 0){
                    nodeToButterflies[neighbor] = 0;
                }
            }

            itr++;
        }

        if (count > 0.0)
        {
            double weight_sum = count * weight;

            map_itr = nodeToButterflies.find(src);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[src] = -weight_sum;
            }else{
                nodeToButterflies[src] = nodeToButterflies[src] - weight_sum; 
            }

            map_itr = nodeToButterflies.find(dst);
            if(map_itr == nodeToButterflies.end()){
                nodeToButterflies[dst] = -weight_sum;
            }else{
                nodeToButterflies[dst] = nodeToButterflies[dst] - weight_sum; 
            }

            globalButterfly -= weight_sum;
        }
    }
}






