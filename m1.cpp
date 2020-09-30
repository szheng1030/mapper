/* 
 * Copyright 2020 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"

#include "m1.h"
#include "m1helper.h"
#include "m2helper.h"
#include "Globals.h"
#include "StreetData.h"
#include "m3.h"

#include <map>
#include <cmath>
#include <cctype>
#include <algorithm>
#include <unordered_set>
#include <time.h>


//Street Segment Class
//Access info with the following call:
//ss_info[STREET_SEGMENT_ID].get(...)
std::vector<ssInfo> ss_info;
 

//Intersection Class
//Access info with the following call:
//inter_info[INTERSECTION_ID].get(...)
std::vector<interInfo> inter_info;


//Street Class
//Access info with the following call:
//street_info[STREET_ID].get(...)
std::vector<streetInfo> street_info;
std::vector<int> large_ss_info;
std::vector<int> med_ss_info;
std::vector<int> small_ss_info;


//Street Name Database
//Access a list of streets ids given a search id
//Search ID = Street name first character ASCII * second character ASCII
std::vector<std::unordered_set<int>> street_name_db;


//hashtables for ordering OSMNodes and OSMWays by their OSMIDs
std::unordered_map<OSMID, OSMNode> OSMNodeTable;
std::unordered_map<OSMID, OSMWay> OSMWayTable;

// Feature Info Class
std::vector<featureInfo> feature_info;
std::vector<int> exlarge_feature_idx;
std::vector<int> large_feature_idx;
std::vector<int> med_feature_idx;
std::vector<int> small_feature_idx;
std::vector<int> exsmall_feature_idx;

//Point of Interest Class
std::vector<pointsOfInterest> points_of_interest;

bool load_map_success;

//Handles loading of both API's. Initializes members in the three classes
//defined in StreetData.h
//Initialization of all global variables is handled here
bool load_map(std::string map_name) {
    //Load StreetsDatabaseAPI
    load_map_success = loadStreetsDatabaseBIN(map_name);
    if (!load_map_success) {
        return load_map_success;
    }
 
    //Load OSMDatabaseAPI
    std::string replace =  "streets";
    map_name.replace(map_name.rfind(replace), replace.size(), "osm");
    bool load_osm_success = loadOSMDatabaseBIN(map_name);
    if (!load_osm_success) {
        return load_osm_success;
    }
    
    //Class vector initializations
    ss_info.resize(getNumStreetSegments());
    inter_info.resize(getNumIntersections());
    street_info.resize(getNumStreets());
    street_name_db.resize(MAX_ASCII_SQAURED);
    feature_info.resize(getNumFeatures());
    points_of_interest.resize(getNumPointsOfInterest());
    
    std::cout << "Initializing intersection database... " << std::endl;
    //std::cout << "Initializing intersection database... " << clock() << std::endl;
    
    //Initialize world bounds for map
    world_bounds_initialization();
 
    //Create global to hold INTERSECTION related info
    construct_intersection_info();
    
    //Calculate final world bounds
    world_bounds_finalization();
    
    std::cout << "Initializing features database... " << std::endl;
    //std::cout << "Starting Feature init: " << clock() << std::endl;
    
    //Create global to hold FEATURE related info
    construct_feature_info();
    
    std::cout << "Initializing OSM database... " << std::endl;
    //std::cout << "Starting OSM init: " << clock() << std::endl;
    
    //Create global to hold OSMNODE related info
    create_OSM_tables();
    
    std::cout << "Initializing streets database... " << std::endl;
    //std::cout << "Starting street init: " << clock() << std::endl;
    
    //Create global to hold STREET SEGMENT related info
    construct_street_segment_info();
    
    std::cout << "Initializing point of interest database... " << std::endl;
    //std::cout << "Starting POI init: " << clock() << std::endl;
    
    //Create global to hold POINT OF INTEREST related info
    //construct_pointofinterest_info();
    
    std::cout << "All databases loaded! Drawing map... "  << std::endl;
    //std::cout << "All loaded: " << clock() << std::endl;
    
    //////// Test travel_time() start ///////
    //const std::vector<StreetSegmentIndex>& path,
    // std::string getStreetName(StreetIndex streetIdx);
    
    //double test_path = compute_path_travel_time(path, 15);
    //std::cout << "Length test path: " << test_path << std::endl;
    ////// Test travel_time() end  //////
    
    std::cout << std::endl;
    std::cout << "Welcome to U|MAP!" << std::endl;
    std::cout << "Input 2 intersections to find their shortest path." <<std::endl;
    std::cout << "Describe intersections by naming the 2 streets that they're made of, with an 'and' in between." <<std::endl;
    std::cout << "If you wish to factor in some initial walking distance, check walk mode." <<std::endl;
    std::cout << "Alternatively, right-click on 2 intersections on the map," <<std::endl;
    std::cout << "This will also display the shortest path between these intersections!" <<std::endl <<std::endl;
    
    
    return load_map_success;
}

//Close both APIs
void close_map() {
    if (load_map_success) {
        ss_info.clear();
        street_info.clear();
        inter_info.clear();
        street_name_db.clear();
        OSMNodeTable.clear();
        OSMWayTable.clear();
        feature_info.clear();
        points_of_interest.clear();
        large_ss_info.clear();
        med_ss_info.clear();
        small_ss_info.clear();
        large_feature_idx.clear();
        med_feature_idx.clear();
        small_feature_idx.clear();
        closeStreetDatabase();
        closeOSMDatabase();  
    }
}
 

//Returns the distance between two coordinates in meters,
//where the two points are passed in as a LatLon pair
double find_distance_between_two_points(std::pair<LatLon, LatLon> points) {
    //Changes pair of (Lat,Lon) into (x,y)
    
    double x_1 = points.first.lon() * DEGREE_TO_RADIAN * cos((points.first.lat() 
        * DEGREE_TO_RADIAN + points.second.lat() * DEGREE_TO_RADIAN) / 2);
    double y_1 = points.first.lat() * DEGREE_TO_RADIAN;
    double x_2 = points.second.lon() * DEGREE_TO_RADIAN * cos((points.first.lat() 
        * DEGREE_TO_RADIAN + points.second.lat() * DEGREE_TO_RADIAN) / 2);
    double y_2 = points.second.lat() * DEGREE_TO_RADIAN;
 
    return (EARTH_RADIUS_METERS * sqrt(pow((y_2 - y_1), 2) + pow((x_2 - x_1), 2)));
    
    /*double x_1 = x_from_lon(points.first.lon());
    double y_1 = y_from_lat(points.first.lat());
    double x_2 = x_from_lon(points.second.lon());
    double y_2 = y_from_lat(points.second.lat());
 
    return (EARTH_RADIUS_METERS * sqrt(pow((y_2 - y_1), 2) + pow((x_2 - x_1), 2)));*/
}
 

//Returns the length of the given street segment in meters
double find_street_segment_length(int street_segment_id) {
    //Direct access via pre-loaded Street Segment class
    return ss_info[street_segment_id].get_length();
}


//Returns the travel time to drive a street segment in seconds
//(time = distance/speed_limit)
double find_street_segment_travel_time(int street_segment_id) {
    //Direct access via pre-loaded Street Segment class
    return ss_info[street_segment_id].get_travel_time();
}


//Returns the nearest intersection to the given position.
int find_closest_intersection(LatLon my_position) {
    
    // half of earth's circumference (max distance from my_position to target intersection)
    double distance_to_closest_intersection = EARTH_RADIUS_METERS;
    double temp_distance = 0;
    int closest_intersection_ID = -1;
    
    // loop through all intersections
    for(int i = 0; i < getNumIntersections() - 1; i++){
        
        // make a LatLon pair of intersection and position and calculate distance between them
        std::pair<LatLon,LatLon> points_pair(my_position, getIntersectionPosition(i));
        temp_distance = find_distance_between_two_points(points_pair);    //(std::pair<LatLon, LatLon> points);
        
        // set new closest_intersection_ID if the distance calculated above is shorter than earlier closest distance
        if(temp_distance <= distance_to_closest_intersection){
            closest_intersection_ID = i;
            distance_to_closest_intersection = temp_distance;
        }
    }
    
    return closest_intersection_ID;
}


//Returns the street segments for the given intersection 
std::vector<int> find_street_segments_of_intersection(int intersection_id) {
    //Direct access via pre-loaded Intersection class
    return inter_info[intersection_id].get_ss_id();
}


//Returns the street names at the given intersection (includes duplicate street 
//names in returned vector)
std::vector<std::string> find_street_names_of_intersection(int intersection_id) {
    //vector to be returned
    std::vector<std::string> streets = {};
    
    //number of street segments that meet at the intersection
    int num_segments = getIntersectionStreetSegmentCount(intersection_id);
    
    InfoStreetSegment segment;
    
    //used to find the streetIDs that the segments belong to
    StreetIndex street_id = 0;
    
    //looping through all segments
    for(int i = 0; i < num_segments; i++){
        //creating an instance of Struct InfoStreetSegment for street segment with index i
        segment = getInfoStreetSegment(getIntersectionStreetSegment(intersection_id, i));
        
        //find index of street this segment belongs to
        street_id = segment.streetID;
        
        //add street_id to the vector streets
        streets.push_back(getStreetName(street_id));
    }
    
    return streets;
}


//Returns true if you can get from intersection_ids.first to intersection_ids.second using a single 
//street segment (hint: check for 1-way streets too)
//corner case: an intersection is considered to be connected to itself
bool are_directly_connected(std::pair<int, int> intersection_ids) {
    
    //connected to itself (corner case)
    if(intersection_ids.first == intersection_ids.second){
        return true;
    }
    
    InfoStreetSegment segment;
    
    //number of street segments that meet at the intersection
    int num_segments = getIntersectionStreetSegmentCount(intersection_ids.first);
    
    //looping through all street segments
    for(int i = 0; i < num_segments; i++){
        //creating an instance of Struct InfoStreetSegment for street segment with index i
        segment = getInfoStreetSegment(getIntersectionStreetSegment(intersection_ids.first, i));
        
        //need to check for OneWay if traveling in to->from direction
        if( (segment.to == intersection_ids.second) 
             || ( (segment.from == intersection_ids.second) && !segment.oneWay) ){
            return true;
        }
    }
    
    return false;
}


//Returns all intersections reachable by traveling down one street segment 
//from given intersection (hint: you can't travel the wrong way on a 1-way street)
//the returned vector should NOT contain duplicate intersections
std::vector<int> find_adjacent_intersections(int intersection_id) {
    
    std::vector<int> intersections;
    std::vector<int> street_segments = inter_info[intersection_id].get_ss_id();
    for (int i = 0; i < street_segments.size(); i++) {
        InfoStreetSegment segment = getInfoStreetSegment(street_segments[i]);
        
        if(!segment.oneWay) {
            if(segment.to == intersection_id
               && (std::find(intersections.begin(), intersections.end(), segment.from) == intersections.end()) ){
                intersections.push_back(segment.from);
            }
            else if(segment.from == intersection_id
                    && (std::find(intersections.begin(), intersections.end(), segment.to) == intersections.end()) ){
                intersections.push_back(segment.to);
            }
        }
        
        else{
            if(segment.from == intersection_id
               && (std::find(intersections.begin(), intersections.end(), segment.to) == intersections.end()) ){
                intersections.push_back(segment.to);
            }
        }
    }
    
    return intersections;
    
    
    
    
    /*//vector of intersections to be returned
    std::vector<int> intersections = {};
    
    //number of segments that meet at the intersection
    int seg_count = getIntersectionStreetSegmentCount(intersection_id);
    
    InfoStreetSegment segment;
    
    //loop through segments of intersection
    for(int i = 0; i < seg_count; i++){
        segment = getInfoStreetSegment(getIntersectionStreetSegment(intersection_id, i));
        
        if(!segment.oneWay){
            if(segment.to == intersection_id
               && (std::find(intersections.begin(), intersections.end(), segment.from) == intersections.end()) 
               && segment.from != intersection_id) {
                intersections.push_back(segment.from);
            }
            else if(segment.from == intersection_id
                    && (std::find(intersections.begin(), intersections.end(), segment.to) == intersections.end()) 
                    && segment.to != intersection_id) {
                intersections.push_back(segment.to);
            }
        }
        // need to consider one ways (can only travel in from -> to directions)
        else{
            if(segment.from == intersection_id
               && (std::find(intersections.begin(), intersections.end(), segment.to) == intersections.end())
                    && segment.to != intersection_id) {
                intersections.push_back(segment.to);
            }
        }
        
        //// old code ////
        //if no elements match, std::find() returns last
        //need to check for OneWay (traveling in to->from direction)
        if(segment.to == intersection_id
           && std::find(intersections.begin(), intersections.end(), segment.from) == intersections.end()
           && !segment.oneWay){
            intersections.push_back(segment.from);
        }
        //do not need to check for OneWay (traveling in from->to direction)
        else if(segment.from == intersection_id 
                && std::find(intersections.begin(), intersections.end(), segment.to) == intersections.end()){
            intersections.push_back(segment.to);
        }*/
        
    //}
    
    //return intersections;
}


//Returns all street segments for the given street
std::vector<int> find_street_segments_of_street(int street_id) {
    //Direct access via pre-loaded global class
    return street_info[street_id].get_ss_id();
}


//Returns all intersections along the a given street
std::vector<int> find_intersections_of_street(int street_id) {
    //Direct access via pre-loaded global class
    return street_info[street_id].get_inter_id();
}


//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
std::vector<int> find_intersections_of_two_streets(std::pair<int, int> street_ids) {
    
    //Return vector
    std::vector<int> ans;
    
    //Get the 2 vectors from direct access via pre-loaded global class
    std::vector<int> inter_id1 = street_info[street_ids.first].get_inter_id();
    std::vector<int> inter_id2 = street_info[street_ids.second].get_inter_id();

    //Check for common intersections within the 2 vectors
    for (int i = 0; i < inter_id1.size(); i++) {
        for (int j = 0; j < inter_id2.size(); j++) {
            if (inter_id1[i] == inter_id2[j]) {
                ans.push_back(inter_id1[i]);
            }
        }
    }
    
    return ans;
}


//Returns all street ids corresponding to street names that start with the given prefix
//Case-insensitive, ignore white-spaces
//For example, both "bloor " and "BloOrst" are prefixes to "Bloor Street East".
//If no streets matches street_prefix, return an empty (length 0) vector.
//Return empty vector if street_prefix is empty
std::vector<int> find_street_ids_from_partial_street_name(std::string street_prefix) {
    
    //Vector to return
    std::vector<int> ans = {};
    
    //corner case (length of prefix is 0)
    if (street_prefix.size() == 0) {
        return ans;
    }
    
    //Remove all spaces in street_prefix to allow comparison with street names
    //Stored in street_name_db, which also has no white-spaces
    //Generate search key from product of 1st and 2nd char of street_prefix
    street_prefix.erase(std::remove(street_prefix.begin(), street_prefix.end(), ' '), street_prefix.end());
    int search_index = street_prefix[0] *  street_prefix[1];
    if(search_index < 0) {
        search_index = -search_index;
    }
   
    //Search in street_name_db for with the generated search key
    //Use case-insensitive comparison to find all streets that have names
    //That include street_prefix within them
    //Construct a new vector of all these streets' ids
    for (auto it = street_name_db[search_index].begin(); it !=  street_name_db[search_index].end(); it++) {
        std::string compare = street_info[*it].get_db_name();
        std::string new_compare = compare.substr(0, street_prefix.size());
        if (caseInSensStringCompare(street_prefix, new_compare)) {
            ans.push_back(*it);
        }
    }
    
    return ans;
}


//Returns the area of the given closed feature in square meters
//Assume a non self-intersecting polygon (i.e. no holes)
//Return 0 if this feature is not a closed polygon.
double find_feature_area(int feature_id) {
    return (feature_info[feature_id].get_closed() ? 
        feature_info[feature_id].get_area() : 0);
}


//Returns the length of the OSMWay that has the given OSMID, in meters.
//Uses OSMDatabaseAPI.h
double find_way_length(OSMID way_id) {
    std::vector<OSMID> target_nodes_id;
    std::vector<LatLon> target_nodes_pos;
    
    double length = 0;

    //Find the OSMIDs of the Nodes of the given OSMWay
    target_nodes_id = getWayMembers(&((*OSMWayTable.find(way_id)).second));
    
    //Find the OSMNodes with the OSMIDs in target_nodes_id
    //Construct a vector of their LatLons
    for (int i = 0; i < target_nodes_id.size(); i++) {
        target_nodes_pos.push_back(getNodeCoords(&((*OSMNodeTable.find(target_nodes_id[i])).second)));
    }
    
    //Use the LatLons to add up all of the distances between each OSMNode
    //Using our find distance function
    for (int i = 0; i < target_nodes_pos.size() - 1; i++) {
        std::pair<LatLon, LatLon> pair(target_nodes_pos[i], target_nodes_pos[i+1]);
        length += find_distance_between_two_points(pair);
    }
    
    return length;
}
