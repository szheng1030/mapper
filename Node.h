/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Node.h
 * Author: zhengsa3
 *
 * Created on March 23, 2020, 1:09 AM
 */

#pragma once
#include "m3.h"
#include "StreetData.h"
#include "Globals.h"

constexpr int ASTAR_HEURISTIC_VALUE = 40;

/* Node structure for A* algorithm*/
struct Node {
public:
    int index;                      // Intersection index
    LatLon position;                // lat and lon coordinates
    double time_from_start;         // g-value
    double time_to_end;             // h-value
    double turn_probability;        // Optimization heuristic to approximate
                                    // If an intersection was going to be 
                                    // Applied a penalty due to a turn
                                    // (Not used)
    double evaluation;              // f-value = g + h
    int parent = -1;                // intersection index of parent node
    int source_segment = -1;        // street segment connecting to parent node

    Node() {}                       // Empty default constructor
    
    Node(int idx) {                 // Need intersection index to create a node
        index = idx;
        position = inter_info[idx].get_position();
    }
};

/* Compare Type for A* */
/* Lowest evaluation value should have highest priority*/
struct CompareNodes {
    bool operator()(Node a, const Node b) {
       return (a.evaluation > b.evaluation);
    }
};

/* Compare Type for Dijkstra */
/* Highest travel time should have highest priority*/
struct CompareNodes_G {
    bool operator()(Node a, const Node b) {
       return (a.time_from_start < b.time_from_start);
    }
};

/* Structure to keep track of elements in the OPEN list */
struct History {
public:
    double time_from_start;
    int duplicates = 0;
    
    History() {}                          // Empty default constructor
    
    History(double time, int count) {     // Need intersection index to create a node
        time_from_start = time;
        duplicates = count;
    }
};
