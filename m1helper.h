/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m1helper.h
 * Author: zhengsa3
 *
 * Created on February 29, 2020, 3:15 PM
 */

#ifndef M1HELPER_H
#define M1HELPER_H

#include "m1.h"

//Constants
constexpr double KMH_TO_MS = 3.6;
constexpr int MAX_ASCII_SQAURED = 16129;

//Case-insensitive compares if 2 characters are the same
//Source: https://thispointer.com/c-case-insensitive-string-comparison-using-stl-c11-boost-library/
bool compareChar(char & c1, char & c2);

//Case-insensitive compares if 2 strings are the same
//Source: https://thispointer.com/c-case-insensitive-string-comparison-using-stl-c11-boost-library/
bool caseInSensStringCompare(std::string & str1, std::string &str2);

//Create OSM tables for OSMNodes and OSMWays
void create_OSM_tables();

//Initialize world bounds for map
void world_bounds_initialization();

//Calculate final world bounds
void world_bounds_finalization();

//Construct a global class to access intersection related info
void construct_intersection_info();

//Construct a global class to access street segment related info
void construct_street_segment_info();

//Construct a global class to access feature related info
void construct_feature_info();

//Construct a global class to access POI related info
void construct_pointofinterest_info();

#endif /* M1HELPER_H */

