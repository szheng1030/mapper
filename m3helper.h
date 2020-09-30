/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3helper.h
 * Author: zhengsa3
 *
 * Created on March 21, 2020, 5:08 AM
 */

#pragma once
#include "Node.h"
#include <unordered_map>

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

double euclidian(LatLon startPos, LatLon endPos);
double manhattan(LatLon startPos, LatLon endPos);

std::vector<int> find_common_segment(int fromIndex, int toIndex, bool ignoreOneway);
std::vector<int> astar(int start, int end, const double penalty);
std::unordered_map<int, Node> mod_dijkstra(int start, const double penalty, double const speed, double const time_limit);
void draw_path(const std::vector<StreetSegmentIndex>& path, ezgl::renderer *g, bool draw_walk);
//std::vector<int> astar_v_turn(int start, int end, const double penalty);
//std::vector<int> astar_v_neighborMin(int start, int end, const double penalty);


