/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "Globals.h"
#include "m2helper.h"
#include "m3helper.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <map>

#include "StreetData.h"
#include "Node.h"
#include <cassert>
#include <cmath>
#include <cctype>
#include <algorithm>
#include <unordered_set>
#include <string>
#include <queue>
#include <set>

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/canvas.hpp"

/* std::vector<int> astar_v_neighborMin(int start, int end, const double penalty) {
    
    std::vector<int> path;
    std::priority_queue<Node, std::vector<Node>, CompareNodes> openList;
    std::unordered_map<int, Node> closedList;
    
    Node startNode(start);
    LatLon endPos = inter_info[end].get_position();
    
    startNode.time_from_start = 0;
    startNode.time_to_end = manhattan(startNode.position, endPos) / 40;
    startNode.evaluation = startNode.time_to_end;
    openList.push(startNode);
    
    while (!openList.empty()) {
        bool flag = false;
        bool update = false;
        Node current = openList.top();
        openList.pop();
        if (closedList.find(current.index) != closedList.end()) {
            flag = true;
            current.neighborMin = closedList[current.index].neighborMin;
        }
        else    
            closedList.insert(std::make_pair(current.index, current));
        
        if (current.index == end) {
            while(current.index != start) {
                path.insert(path.begin(), current.source_segment);
                current = closedList[current.parent];
            }
            std::cout << "Path Found" << std::endl;
            return path;
        }
        
        std::vector<int> neighbors = find_adjacent_intersections(current.index);
        
        for (int neighbor : neighbors) {
            if (closedList.find(neighbor) != closedList.end())
               continue;
            
            Node neighborNode(neighbor);
            neighborNode.parent = current.index;
            
            std::vector<int> common_segment = find_common_segment(current.index, neighborNode.index, false);
            if (common_segment.size() > 1) {
                int index = -1;
                double min_time = 99999999;
                
                for (int segment : common_segment) {
                    double temp_time = ss_info[segment].get_travel_time();
                    if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[segment].get_street_id())
                        temp_time += penalty;
                    if (temp_time < min_time) {
                        min_time = temp_time;
                        index = segment;
                    }
                }
                neighborNode.source_segment = index;
            }
            else 
                neighborNode.source_segment = common_segment[0];
            
            assert(neighborNode.source_segment != -1);
            
            neighborNode.time_from_start = current.time_from_start + ss_info[neighborNode.source_segment].get_travel_time();
            if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[neighborNode.source_segment].get_street_id())
                neighborNode.time_from_start += penalty;
            
            
            neighborNode.time_to_end = manhattan(neighborNode.position, endPos) / 40;
            neighborNode.evaluation = neighborNode.time_from_start + neighborNode.time_to_end;
            
            if (neighborNode.time_from_start < current.neighborMin) {
                current.neighborMin = neighborNode.time_from_start;
                closedList[current.index].neighborMin = current.neighborMin;
                update = true;
            }
            
            if (flag && update) {
                if (current.index == 88267)
                std::cout << "closed list update" << std::endl;
                closedList[current.index].parent = current.parent;
                closedList[current.index].source_segment = current.source_segment;
            }
            
            openList.push(neighborNode);
        }
    }
    
    assert(closedList.empty());
    assert(path.empty());
    std::cout << "Path not found" << std::endl;
    return path;
} 

std::vector<int> astar_v_turn(int start, int end, const double penalty) {
    
    std::vector<int> path;
    std::priority_queue<Node, std::vector<Node>, CompareNodes> openList;
    std::unordered_map<int, bool> openProbability;
    std::unordered_map<int, Node> closedList;
    
    Node startNode(start);
    LatLon endPos = inter_info[end].get_position();
    
    startNode.time_from_start = 0;
    startNode.time_to_end = manhattan(startNode.position, endPos) / 40;
    startNode.evaluation = startNode.time_to_end;
    openList.push(startNode);
    
    while (!openList.empty()) {
        Node current = openList.top();
        openList.pop();
        closedList.insert(std::make_pair(current.index, current));
        
        if (current.index == end) {
            while(current.index != start) {
                path.insert(path.begin(), current.source_segment);
                current = closedList[current.parent];
            }
            std::cout << "Path Found" << std::endl;
            return path;
        }
        
        std::vector<int> neighbors = find_adjacent_intersections(current.index);
        
        for (int neighbor : neighbors) {
            if (closedList.find(neighbor) != closedList.end())
               continue;
            
            Node neighborNode(neighbor);
            neighborNode.parent = current.index;
            
            std::vector<int> common_segment = find_common_segment(current.index, neighborNode.index, false);
            if (common_segment.size() > 1) {
                int index = -1;
                double min_time = 99999999;
                
                for (int segment : common_segment) {
                    double temp_time = ss_info[segment].get_travel_time();
                    if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[segment].get_street_id())
                        temp_time += penalty;
                    if (temp_time < min_time) {
                        min_time = temp_time;
                        index = segment;
                    }
                }
                neighborNode.source_segment = index;
            }
            else 
                neighborNode.source_segment = common_segment[0];
            
            assert(neighborNode.source_segment != -1);
            
            neighborNode.time_from_start = current.time_from_start + ss_info[neighborNode.source_segment].get_travel_time();
            if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[neighborNode.source_segment].get_street_id())
                neighborNode.time_from_start += penalty;
            
            
            std::vector<int> adjacent = find_adjacent_intersections(neighborNode.index);
            int matching_segments = 0;
            int size = adjacent.size();
            int most_probable = -1;
            double min_gScore = 999999;
            for (int i = 0; i < size; i++) {
                if (adjacent[i] == neighborNode.parent)
                    adjacent.erase(adjacent.begin() + i);
                else {
                    int common = find_common_segment(neighborNode.index, adjacent[i], false)[0];
                    double temp_gScore = neighborNode.time_from_start + ss_info[common].get_travel_time();
                    if (ss_info[common].get_street_id() != ss_info[neighborNode.source_segment].get_street_id())
                        matching_segments++;
                    if (closedList.find(adjacent[i]) == closedList.end() && openProbability[adjacent[i]] == false) {
                        if (temp_gScore < min_gScore) {
                            min_gScore = temp_gScore;
                            most_probable = adjacent[i];
                        }
                    }
                }
            }
            
            if (matching_segments == adjacent.size())
                neighborNode.turn_probability = penalty;
            else if (matching_segments == adjacent.size() - 1)
                neighborNode.turn_probability = penalty / 2;
            if (most_probable != -1) {
                int common = find_common_segment(neighborNode.index, most_probable, false)[0];
                if (ss_info[common].get_street_id() != ss_info[neighborNode.source_segment].get_street_id())
                    neighborNode.turn_probability = penalty * 2;
            }
            
            
            neighborNode.time_to_end = manhattan(neighborNode.position, endPos) / 40;
            neighborNode.evaluation = neighborNode.time_from_start + neighborNode.time_to_end + neighborNode.turn_probability;
            
            openProbability[neighborNode.index] = true;
            openList.push(neighborNode);
        }
    }
    
    assert(closedList.empty());
    assert(path.empty());
    std::cout << "Path not found" << std::endl;
    return path;
} */

/*
std::vector<int> astar_original(int start, int end, const double penalty, double H) {

    std::vector<int> path;
    
    if (start == end) {
        std::cout << "Already at destination" << std::endl;
        return path;
    }

    std::priority_queue<Node, std::vector<Node>, CompareNodes> unexploredNodes;
    std::unordered_map<int, double> unexploredHistory;
    std::unordered_map<int, Node> exploredNodes;

    Node startNode(start);
    Node endNode(end);
    LatLon endPos = inter_info[end].get_position();

    startNode.time_from_start = 0;
    startNode.time_to_end = (H != 0) ? manhattan(startNode.position, endPos) / H : 0;
    startNode.evaluation = startNode.time_to_end;
    unexploredNodes.push(startNode);

    while (!unexploredNodes.empty()) {

        Grab node with lowest f 
        Node current = unexploredNodes.top();
        Insert into closed list 
        exploredNodes.insert(std::make_pair(current.index, current));
        Removed from open list 
        unexploredNodes.pop();
        unexploredHistory.erase(current.index);
        
        Check if current node is the destination 
        if (current.index == end) {
            while (current.index != start) {
                path.insert(path.begin(), current.source_segment);
                current = exploredNodes[current.parent];
                
            }
            std::cout << "Path Found" << std::endl;
            return path;
        }

        Find all adjacent intersections of current node 
        std::vector<int> adj = find_adjacent_intersections(current.index);

        if (!adj.empty()) {
            For each adjacent intersection 
            for (auto it = adj.begin(); it != adj.end(); it++) {
                
                Create a new node and assign it's parent to current, find their connecting street segment 
                Node child(*it);
                child.parent = current.index;
                std::vector<int> common_segment = find_common_segment(current.index, child.index, false);
                if (common_segment.size() > 1) {
                    int index = -1;
                    double value = 99999;
                    for (auto it2 = common_segment.begin(); it2 != common_segment.end(); it2++) {
                        double temp_value = ss_info[*it2].get_travel_time();
                        if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[*it2].get_street_id())
                            temp_value += penalty;
                        if (temp_value < value) {
                            value = temp_value;
                            index = *it2;
                        }
                    }
                    child.source_segment = index;
                    
                }
                else
                    child.source_segment = common_segment[0];
                assert(child.source_segment != -1);
                
                Find child's g, h, f values 
                std::vector<int> temp_vector = {child.source_segment};     
                
                if (exploredNodes.find(current.index) != exploredNodes.end())
                    child.time_from_start = exploredNodes[current.index].time_from_start + compute_path_travel_time(temp_vector, penalty);
                else
                    child.time_from_start = compute_path_travel_time(temp_vector, penalty);
                
                
                if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[child.source_segment].get_street_id()) {
                    
                    child.time_from_start += penalty;
                }
                
                child.time_to_end = (H != 0) ? manhattan(child.position, endPos) / H : 0;
                child.evaluation = child.time_from_start + child.time_to_end;
                
                if (unexploredHistory.find(*it) != unexploredHistory.end()) {
                        if (unexploredHistory[*it] <= child.time_from_start)
                            continue;
                    }
                auto explored = exploredNodes.find(*it);
                if (explored != exploredNodes.end()) {
                    if ((*explored).second.time_from_start <= child.time_from_start)
                        continue;
                    else {
                        exploredNodes.erase(explored);
                    }
                }

                unexploredNodes.push(child);
                unexploredHistory.insert(std::make_pair(child.index, child.time_from_start));

            }
        }
 
    }

    assert(unexploredNodes.empty());
    assert(path.empty());
    std::cout << "Path not found" << std::endl;
    return path;
}


std::unordered_map<int, Node> dijkstra_v2(int start, const double penalty, double const speed, double const time_limit) {
    
    std::vector<int> top10;
    
    std::priority_queue<Node, std::vector<Node>, CompareNodes> openList;
    std::unordered_map<int, Node> closedList;
    
    Node startNode(start);
    
    startNode.time_from_start = 0;
    openList.push(startNode);
    
    while (!openList.empty()) {
        Node current = openList.top();
        openList.pop();  
        closedList.insert(std::make_pair(current.index, current));
        
        std::vector<int> neighbors = find_adjacent_intersections(current.index);
        
        for (int neighbor : neighbors) {
            if (closedList.find(neighbor) != closedList.end())
               continue;
            
            Node neighborNode(neighbor);
            neighborNode.parent = current.index;
            
            std::vector<int> common_segment = find_common_segment(current.index, neighborNode.index, true);
            if (common_segment.size() > 1) {
                int index = -1;
                double min_time = 99999999;
                
                for (int segment : common_segment) {
                    double temp_time = ss_info[segment].get_length() / speed;
                    if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[segment].get_street_id())
                        temp_time += penalty;
                    if (temp_time < min_time) {
                        min_time = temp_time;
                        index = segment;
                    }
                }
                neighborNode.source_segment = index;
            }
            else 
                neighborNode.source_segment = common_segment[0];
            
            assert(neighborNode.source_segment != -1);
            
            neighborNode.time_from_start = current.time_from_start + ss_info[neighborNode.source_segment].get_length() / speed;
            if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[neighborNode.source_segment].get_street_id())
                neighborNode.time_from_start += penalty;
            
            if (neighborNode.time_from_start > time_limit)
                continue;

            openList.push(neighborNode);
        }
    }
    
    return closedList;
}*/
