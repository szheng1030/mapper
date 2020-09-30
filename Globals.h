/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Globals.h
 * Author: zhengsa3
 *
 * Created on February 19, 2020, 1:44 PM
 */

#pragma once

#include "StreetData.h"
#include <unordered_map>

//Fundemental data structures, holds all street data
extern std::vector<ssInfo> ss_info;
extern std::vector<interInfo> inter_info;
extern std::vector<streetInfo> street_info;
extern std::vector<featureInfo> feature_info;
extern std::vector<pointsOfInterest> points_of_interest;
extern std::vector<std::unordered_set<int>> street_name_db;
extern std::unordered_map<OSMID, OSMNode> OSMNodeTable;
extern std::unordered_map<OSMID, OSMWay> OSMWayTable;

//Global bounds for world to screen conversion
extern double avg_lat, min_lat, max_lat, max_lon, min_lon;
extern double map_height;

//Index classification for street segments container
extern std::vector<int> large_ss_info;
extern std::vector<int> med_ss_info;
extern std::vector<int> small_ss_info;

//Index classification for feature container
extern std::vector<int> exlarge_feature_idx;
extern std::vector<int> large_feature_idx;
extern std::vector<int> med_feature_idx;
extern std::vector<int> small_feature_idx;
extern std::vector<int> exsmall_feature_idx;

//true = walk to car, false = just car
extern bool walk_mode;

//To keep track of the set of intersections that make up
//the most recent queried path
extern std::vector<int> last_path_intersections;

extern const double TURN_PENALTY;

extern std::vector<StreetSegmentIndex> PATH;

extern std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> PATH_WALK_AND_DRIVE;