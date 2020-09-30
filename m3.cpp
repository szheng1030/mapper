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
#include <unordered_map>
#include <string>
#include <queue>
#include <set>

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/canvas.hpp"

std::vector<int> last_path_intersections;
const double TURN_PENALTY = 15.0;

std::vector<StreetSegmentIndex> PATH;
std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> PATH_WALK_AND_DRIVE;
bool walk_mode;


// Returns the time required to travel along the path specified, in seconds.
// The path is given as a vector of street segment ids, and this function can
// assume the vector either forms a legal path or has size == 0.  The travel
// time is the sum of the length/speed-limit of each street segment, plus the
// given turn_penalty (in seconds) per turn implied by the path.  If there is
// no turn, then there is no penalty. Note that whenever the street id changes
// (e.g. going from Bloor Street West to Bloor Street East) we have a turn.
double compute_path_travel_time(const std::vector<StreetSegmentIndex>& path, 
                                const double turn_penalty){
    // return value
    double travel_time = 0;
    
    //corner case: length 0
    if(path.empty()){
        return travel_time;
    }
    
    //starting street
    int current_street_id = ss_info[path[0]].get_street_id();
    
    //loop through the vector of street segments
    for(const auto& it : path){
        travel_time += ss_info[it].get_travel_time();
        
        // check for turn penalty (different street_id)
        if(ss_info[it].get_street_id() != current_street_id){
            current_street_id = ss_info[it].get_street_id();
            travel_time += turn_penalty;
        }
    }
    
    return travel_time;
    
}



// Returns the time required to "walk" along the path specified, in seconds.
// The path is given as a vector of street segment ids. The vector can be of
// size = 0, and in this case, it the function should return 0. The walking
// time is the sum of the length/<walking_speed> for each street segment, plus
// the given turn penalty, in seconds, per turn implied by the path. If there
// is no turn, then there is no penalty.  As mentioned above, going from Bloor
// Street West to Bloor street East is considered a turn
double compute_path_walking_time(const std::vector<StreetSegmentIndex>& path, 
                                 const double walking_speed, 
                                 const double turn_penalty){
    // return value;
    double walking_time = 0;
    
    // corner case: length 0
    if(path.empty()){
        return walking_time;
    }
    
    //starting street
    int current_street_id = ss_info[path[0]].get_street_id();
    
    //loop through the vector of street segments
    for(const auto& it : path){
        walking_time += ss_info[it].get_length() / walking_speed;
        
        // check for turn penalty (different street_id)
        if(ss_info[it].get_street_id() != current_street_id){
            current_street_id = ss_info[it].get_street_id();
            walking_time += turn_penalty;
        }
    }
    
    return walking_time;
}


std::vector<StreetSegmentIndex> find_path_between_intersections(
    const IntersectionIndex intersect_id_start, 
    const IntersectionIndex intersect_id_end,
    const double turn_penalty) {
    
    std::vector<int> normal = astar(intersect_id_start, intersect_id_end, turn_penalty);
        return normal;
}

std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> 
         find_path_with_walk_to_pick_up(
                          const IntersectionIndex start_intersection, 
                          const IntersectionIndex end_intersection,
                          const double turn_penalty,
                          const double walking_speed, 
                          const double walking_time_limit) {
    
    std::unordered_map<int, Node> walking_intersections = mod_dijkstra(start_intersection, turn_penalty, walking_speed, walking_time_limit);
    std::priority_queue<Node, std::vector<Node>, CompareNodes_G> maxQueue;
    for (auto it = walking_intersections.begin(); it != walking_intersections.end(); it++)
        maxQueue.push((*it).second);
    
    std::vector<int> walking_path;
    std::vector<int> driving_path;
    
    int pickup_intersection = -1;
    double min_time = 99999999;
    std::vector<int> last_path_inter;

    for (int i = 0; i < 10; i++) {
        Node newNode(maxQueue.top().index);
        maxQueue.pop();
        std::vector<int> temp_driving_path = astar(newNode.index, end_intersection, turn_penalty);
        
        if (!temp_driving_path.empty()) {
            double temp_time = compute_path_travel_time(temp_driving_path, turn_penalty);
            if (temp_time < min_time) {
                min_time = temp_time;
                driving_path = temp_driving_path;
                last_path_inter.clear();
                last_path_inter = last_path_intersections;
                pickup_intersection = newNode.index;
            }
        }
    }

    assert(pickup_intersection != -1);
    
    Node current = walking_intersections[pickup_intersection];
    while (current.index != start_intersection) {
        walking_path.insert(walking_path.begin(), current.source_segment);
        last_path_inter.insert(last_path_inter.begin(), current.index);
        current = walking_intersections[current.parent];
    }
    last_path_inter.insert(last_path_inter.begin(), start_intersection);
    
    last_path_intersections.clear();
    last_path_intersections = last_path_inter;
    return std::pair<std::vector<int>, std::vector<int>> (walking_path, driving_path);
}




// Dijkstra algorithm for finding the intersections suitable to start driving after walking
std::unordered_map<int, Node> mod_dijkstra(int start, const double penalty, double const speed, double const time_limit) {

    /* Queue for OPEN list*/
    std::priority_queue<Node, std::vector<Node>, CompareNodes> unexploredNodes;
    /* Secondary map to keep track of nodes that have been in the OPEN list */
    std::unordered_map<int, History> unexploredHistory;
    /* Map for CLOSED list*/
    std::unordered_map<int, Node> exploredNodes;

    /* Generate start node, evaluate, and push into OPEN lsit*/
    Node startNode(start);
    startNode.time_from_start = 0;
    unexploredNodes.push(startNode);

    /* Repeat while OPEN list is non-empty*/
    while (!unexploredNodes.empty()) {

        /* Grab node with lowest f */
        Node current = unexploredNodes.top();

        /* Insert into closed list */
        exploredNodes.insert(std::make_pair(current.index, current));
        unexploredNodes.pop();

        /* Find all adjacent intersections of current node */
        std::vector<int> adj = find_adjacent_intersections(current.index);

        if (!adj.empty()) {
            /* For each adjacent intersection */
            for (auto it = adj.begin(); it != adj.end(); it++) {
 
                /* Create a new node and assign it's parent to current, find their connecting street segment */
                Node child(*it);
                child.parent = current.index;
                std::vector<int> common_segment = find_common_segment(current.index, child.index, true);
                
                /* Double check if two intersections have more than 1 segment connecting them*/
                if (common_segment.size() > 1) {
                    int index = -1;
                    double value = 99999;
                    for (auto it2 = common_segment.begin(); it2 != common_segment.end(); it2++) {
                        double temp_value = ss_info[*it2].get_length() / speed;
                        if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[*it2].get_street_id())
                            temp_value += penalty;
                        if (temp_value < value) {
                            value = temp_value;
                            index = *it2;
                        }
                    }
                    /* Store segment from which the intersection is derived from*/
                    child.source_segment = index;
                }
                else
                    child.source_segment = common_segment[0];
                
                /* Make sure we have a path to follow back to*/
                assert(child.source_segment != -1);
                
                /* Find child's g values */
                std::vector<int> temp_vector = {child.source_segment};     
                if (exploredNodes.find(current.index) != exploredNodes.end())
                    child.time_from_start = exploredNodes[current.index].time_from_start + compute_path_walking_time(temp_vector, speed, penalty);
                else
                    child.time_from_start = compute_path_walking_time(temp_vector, speed, penalty);
                /* Consider turn penalty*/
                if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[child.source_segment].get_street_id()) {
                    child.time_from_start += penalty;
                }
                
                /* Check if within given time limit*/
                if (child.time_from_start > time_limit)
                    continue;
                
                /* Check if neighboring node has been in the OPEN list*/
                if (unexploredHistory.find(*it) != unexploredHistory.end()) {
                    /* Skip adding if the one currently in the OPEN list is much better*/
                    if (unexploredHistory[*it].time_from_start <= child.time_from_start)
                            continue;
                    }
                
                /* Check if neighboring node is in the CLOSED list*/
                /* Reassgin CLOSED list if previous iterations had non-optimal paths*/
                auto explored = exploredNodes.find(*it);
                if (explored != exploredNodes.end()) {
                    if ((*explored).second.time_from_start <= child.time_from_start)
                        continue;
                    else {
                        exploredNodes.erase(explored);
                    }
                }
                
                /* Push node onto the OPEN list*/
                unexploredNodes.push(child);
                History newHistory(child.time_from_start, 0);
                unexploredHistory.insert(std::make_pair(child.index, newHistory));
            }
        }
    }
    /* return the map of nodes that have been explored*/
    return exploredNodes;
}


/* A* algorithm for finding shortest path driving distance between 2 intersections*/
std::vector<int> astar(int start, int end, const double penalty) {
    
    /* Final path vector (to be returned)*/
    std::vector<int> path;
    
    /* Priority Queue for OPEN list*/
    std::priority_queue<Node, std::vector<Node>, CompareNodes> openList;
    /* Unordered Map for CLOSED list*/
    std::unordered_map<int, Node> closedList;
    
    /* Evaluate and add start node to open list*/
    Node startNode(start);
    LatLon endPos = inter_info[end].get_position();
    
    startNode.time_from_start = 0;
    startNode.time_to_end = manhattan(startNode.position, endPos) / ASTAR_HEURISTIC_VALUE;
    startNode.evaluation = startNode.time_to_end;
    openList.push(startNode);
    
    /* Repeat until path is found (or not found) */
    while (!openList.empty()) {
        /* Extract the node with the lowest f-Score (evaluation) */
        /* Remove off OPEN list, move into CLOSED list */
        Node current = openList.top();
        openList.pop();  
        closedList.insert(std::make_pair(current.index, current));
        
        /* If the current node is the destination, reconstruct path */
        if (current.index == end) {
            last_path_intersections.clear();
            while(current.index != start) {
                path.insert(path.begin(), current.source_segment);
                last_path_intersections.insert(last_path_intersections.begin(), current.index);
                current = closedList[current.parent];
            }
            last_path_intersections.insert(last_path_intersections.begin(), start);
            //std::cout << "Path Found" << std::endl;
            return path;
        }
        
        /* Find all neighboring intersections of the current node */
        std::vector<int> neighbors = find_adjacent_intersections(current.index);
        
        /* If the neighbor is already in the closed list, ignore */
        for (int neighbor : neighbors) {
            if (closedList.find(neighbor) != closedList.end())
               continue;
            
            /* Set neighbor's parent node to current node*/
            Node neighborNode(neighbor);
            neighborNode.parent = current.index;
            
            /* Find the common segment between the current node and the neighboring node*/
            /* Check special case if 2 segments connect the 2 intersections */
            /* Record the common segment as the segment that the path follows */
            /* From the current node to the neighboring node*/
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
            /* Making sure that a path is traceable */
            assert(neighborNode.source_segment != -1);
            
            /* Evaluate the gScore (time from start) of the neighbor node */
            neighborNode.time_from_start = current.time_from_start + ss_info[neighborNode.source_segment].get_travel_time();
            /* Append penalty if needed */
            if (current.source_segment != -1 && ss_info[current.source_segment].get_street_id() != ss_info[neighborNode.source_segment].get_street_id())
                neighborNode.time_from_start += penalty;
            
            /* Evaluate heuristic, hScore (time to end) and final fScore (evaluation) */
            /* And move into OPEN list */
            neighborNode.time_to_end = manhattan(neighborNode.position, endPos) / ASTAR_HEURISTIC_VALUE;
            neighborNode.evaluation = neighborNode.time_from_start + neighborNode.time_to_end;

            openList.push(neighborNode);
        }
    }
    /* If no path is found, make sure that the return vector is empty */
    /* And make sure that all elements in the OPEN list have been searched*/
    assert(openList.empty());
    assert(path.empty());
    std::cout << "Path not found" << std::endl;
    return path;
}


void draw_path(const std::vector<StreetSegmentIndex>& path, ezgl::renderer *g, bool draw_walk){
    
    if(path.size() == 0){
        return;
    }
    
    // Variables used to determine arrow position and direction
    double angle = 0;
    double x_mid = 0;
    double y_mid = 0;
    int counter = 0;
    
    // Starting street
    int current_street_id = ss_info[path[0]].get_street_id();
    
    // Set specifications for street segments
    if(!draw_walk)
        g-> set_color(ezgl::RED);
    else
        g-> set_color(ezgl::BLUE);
    
    g-> set_line_width(2);
    
    //// LOOP THROUGH PATH START ////
    for(const auto& it : path){
        
        //// DRAW STREET SEGMENTS ////
        for(int j = 0; j < ss_info[it].get_xy().size() - 1; j++){
            
            if(map_height < 0.003 && map_height > 0.0005)
                g-> set_line_width(2);
            else if(map_height <= 0.0005)
                g-> set_line_width(3);
            
            g -> draw_line(ss_info[it].get_xy()[j], ss_info[it].get_xy()[j+1]);
        }
        //////////////////////////////
        
        
        //// CHECK FOR DIFFERENT STREET ID START ////
        if( (counter % 3) == 0){
            
            int intersection_1 = last_path_intersections[counter];
            int intersection_2 = last_path_intersections[counter + 1];
            
            draw_arrow_between_intersections(x_mid, y_mid, angle, intersection_1, intersection_2);
            
            //resize arrows
            if(map_height < 0.003 && map_height > 0.0005)
                g-> set_font_size(10);
            else if(map_height <= 0.0005)
                g-> set_font_size(20);
            
            //// DRAW ARROWS ////
            g-> set_color(ezgl::BLACK);
            g-> set_text_rotation(angle);
            g-> draw_text({x_mid, y_mid}, "►");
            /////////////////////
            
            // set colour back to RED/BLUE for street segments
            if(!draw_walk)
                g-> set_color(ezgl::RED);
            else
                g-> set_color(ezgl::BLUE);
        }
        //// CHECK FOR DIFFERENT STREET ID END ////
        
        counter++;
    }
    //// LOOP THORUGH PATH END ////
}
