#include <limits>
#include <random>
#include <algorithm>
#include "TriestFD.hpp"

void TriestFD::processAddition(VertexID src, VertexID dst)
{
    processEdge(src, dst, true);
}

void TriestFD::processDeletion(VertexID src, VertexID dst)
{
    processEdge(src, dst, false);
}

void TriestFD::processEdge(VertexID src, VertexID dst, bool add)
{

    if (src == dst)
    { // ignore self loop
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

    bool isSample = false;
    if (add)
    {
        // sample edge start
        if (ng + nb == 0)
        {
            if (edgeToIndex.size() < k)
            {
                addEdge(src, dst);
                isSample = true;
            }
            else if (random_ratio < k)
            {
                std::mt19937_64 key_selection_eng(rd());
                std::uniform_int_distribution<ui> uniform_key_dis(0, edgeToIndex.size() - 1);
                ui index = uniform_key_dis(key_selection_eng);
                deleteEdge(samples[0][index], samples[1][index]); // remove a random edge from the samples
                addEdge(src, dst);                                // store the sampled edge
                isSample = true;
            }
        }
        else if (random_compensation < nb)
        {
            addEdge(src, dst); // store the sampled edge
            nb--;
            isSample = true;
        }
        else
        {
            ng--;
        }
    }
    else
    {
        KeyID key = ((KeyID)src * std::numeric_limits<unsigned int>::max()) + dst;
        if (edgeToIndex.find(key) != edgeToIndex.end())
        {
            deleteEdge(src, dst); // remove the edge from the samples
            nb++;
            isSample = true;
        }
        else
        {
            ng++;
        }
    }

    if (add)
    {
        s++;
    }
    else
    {
        s--;
    }

    if (isSample)
    {
        count_triangles(src, dst, add); // count the added or deleted triangles
    }
    return;
}

void TriestFD::addEdge(VertexID src, VertexID dst)
{

    ui sampleNum = edgeToIndex.size();
    samples[0][sampleNum] = src;
    samples[1][sampleNum] = dst;
    KeyID key = ((KeyID)src * std::numeric_limits<unsigned int>::max()) + dst;

    edgeToIndex.emplace(key, sampleNum);
    if (srcToDsts.find(key) == srcToDsts.end())
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

void TriestFD::deleteEdge(VertexID src, VertexID dst)
{

    ui sampleNum = edgeToIndex.size(), index;
    KeyID key = ((KeyID)src * std::numeric_limits<unsigned int>::max()) + dst;
    std::unordered_map<KeyID, ui>::iterator edge_idx_itr = edgeToIndex.find(key);
    if (edge_idx_itr != edgeToIndex.end())
    {
        index = edge_idx_itr->second;
        edgeToIndex.erase(edge_idx_itr);
    }

    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator itr = srcToDsts.find(src);
    if (itr != srcToDsts.end())
    {
        (itr->second).erase(dst);
        if ((itr->second).empty())
        {
            srcToDsts.erase(itr);
        }
    }

    itr = srcToDsts.find(dst);
    if (itr != srcToDsts.end())
    {
        (itr->second).erase(src);
        if ((itr->second).empty())
        {
            srcToDsts.erase(itr);
        }
    }

    if (index < sampleNum - 1)
    {
        int newSrc = samples[0][index] = samples[0][sampleNum - 1];
        int newDst = samples[1][index] = samples[1][sampleNum - 1];
        long newKey = ((long)newSrc * std::numeric_limits<unsigned int>::max()) + newDst;
        edgeToIndex.emplace(newKey, index);
    }
}

double TriestFD::getGlobalTriangle()
{
    return globalTriangle;
}

std::unordered_map<VertexID, double> TriestFD::getLocalTriangle()
{
    return nodeToTriangles;
}

void TriestFD::count_triangles(VertexID src, VertexID dst, bool add)
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

    double weight = std::max((s + nb + ng + 0.0) / k * (s + nb + ng - 1.0) / (k - 1.0) * (s + nb + ng - 2.0) / (k - 2.0), 1.0);

    if (add)
    {
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
