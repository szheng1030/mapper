/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StreetData.h
 * Author: jarosson
 *
 * Created on February 3, 2020, 11:59 PM
 */

#pragma once

#include <unordered_set>
#include "LatLon.h"
#include "StreetsDatabaseAPI.h"
#include "ezgl/point.hpp"

/*
 * This header file declares 3 classes that will aid in the initialization of
 * API data pertaining to street segments(ssInfo), intersections(InterInfo) and
 * streets(StreetInfo)
 * The members of these 3 classes will be initialized in load_map() in m1.cpp,
 * hence load_map() is a friend of all 3 classes.
 * The 3 classes are only used to access data, therefore only accessor functions
 * are needed.
 */


//Street classifications parsed from OSM
enum Roads
{
    Motorway = 0,
    Trunk,
    Primary,
    Secondary,
    Tertiary,
    Unclassified,
    Residential,
    Other
};

//Street Segment Class
//Access info with the following call:
//ss_info[STREET_SEGMENT_ID].get(...)
class ssInfo{
private:
    double length;
    double speed_limit;
    double travel_time;
    bool one_way;
    OSMID way_id;
    Roads road_type;
    int street_id;
    std::string street_name;
    std::vector<LatLon> points;
    std::vector<ezgl::point2d> xy;
 
public:
    //Access Functions
    double get_length() { return length; }
    double get_speed_limit() { return speed_limit; }
    double get_travel_time() { return travel_time; }
    bool get_one_way() { return one_way; }
    OSMID get_way_id() { return way_id; }
    Roads get_road_type() { return road_type; }
    int get_street_id() { return street_id; }
    std::string get_street_name() { return street_name; }
    std::vector<LatLon> get_points() { return points; }
    std::vector<ezgl::point2d> get_xy() { return xy; }
   
    //Mutate Functions
    void set_xy(ezgl::point2d _xy) { xy.push_back(_xy); }
    
    friend void construct_intersection_info();
    friend void construct_street_segment_info();
    friend void construct_feature_info();
    friend bool load_map(std::string map_name);
};
 

//Intersection Class
//Access info with the following call:
//inter_info[INTERSECTION_ID].get(...)
class interInfo{
private:
    std::vector<int> ss_id;
    LatLon position;
    bool highlight = false;
    
public:
    //Access Functions
    std::vector<int> get_ss_id() { return ss_id; }
    LatLon get_position() { return position; }
    bool get_highlight() { return highlight; }
    
    //Mutate Functions
    void set_highlight(bool x) { highlight = x; }
 
    friend void construct_intersection_info();
    friend void construct_street_segment_info();
    friend void construct_feature_info();
    friend bool load_map(std::string map_name);
};


//Street Class
//Access info with the following call:
//street_info[STREET_ID].get(...)
class streetInfo {
private:
    double length = 0;
    std::string name;       //Original Street Name
    std::string db_name;    //No white-space name
    std::vector<int> ss_id;
    std::unordered_set<int> inter_id;
 
public:
    //Access Functions
    double get_length() { return length; }
    std::string get_name() { return name; }
    std::string get_db_name() { return db_name; }
    std::vector<int> get_ss_id() { return ss_id; }
    std::vector<int> get_inter_id() {
        std::vector<int> ans(inter_id.begin(), inter_id.end());
        return ans;
    }
 
    friend void construct_intersection_info();
    friend void construct_street_segment_info();
    friend void construct_feature_info();
    friend bool load_map(std::string map_name);
};

class featureInfo {
private:
    FeatureType type;
    bool closed = false;
    double area = 0.0;
    int pointCount;
    OSMID id;
    std::vector<LatLon> points;
    std::vector<ezgl::point2d> xy;
    
public:
    //Access Functions
    FeatureType get_type() { return type; }
    bool get_closed() { return closed; }
    double get_area() { return area; }
    int get_pointCount() { return pointCount; }
    OSMID get_osmid() { return id; }
    std::vector<LatLon> get_points() { return points; }
    std::vector<ezgl::point2d> get_xy() { return xy; }
    
    friend void construct_intersection_info();
    friend void construct_street_segment_info();
    friend void construct_feature_info();
    friend bool load_map(std::string map_name);
};


class pointsOfInterest{
private:
    std::string type;
    std::string name;
    LatLon pos;
    OSMID id;
public:
    std::string get_type() { return type; }
    std::string get_name() { return name; }
    LatLon get_position() { return pos; }
    OSMID get_osmid() { return id; }

    friend void construct_pointofinterest_info();
};