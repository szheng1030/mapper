/* 
 * File:   m1helper.h
 * Author: zhengsa3
 *
 * Created on March 7, 2020, 4:20 AM
 */

#include "m1.h"
#include "m1helper.h"
#include "m2helper.h"
#include "m3helper.h"
#include "Globals.h"

#include "StreetsDatabaseAPI.h"

#include <cassert>
#include <cmath>
#include <cctype>
#include <algorithm>


//Case Insensitive Character Comparison for comparing street names
//Source: https://thispointer.com/c-case-insensitive-string-comparison-using-stl-c11-boost-library/
bool compareChar(char & c1, char & c2) {
	if (c1 == c2)
		return true;
	else if (std::toupper(c1) == std::toupper(c2))
		return true;
	return false;
}


//Case Insensitive String Comparison for comparing street names
//Source: https://thispointer.com/c-case-insensitive-string-comparison-using-stl-c11-boost-library/
bool caseInSensStringCompare(std::string & str1, std::string &str2) {
	return ((str1.size() == str2.size()) 
                && std::equal(str1.begin(), str1.end(), str2.begin(), &compareChar));
}


double x_from_lon(double lon) {
    return lon * DEGREE_TO_RADIAN * cos(avg_lat * DEGREE_TO_RADIAN);
}


double y_from_lat(double lat) {
    return lat * DEGREE_TO_RADIAN;
}


double lon_from_x(double x) {
    return x / (DEGREE_TO_RADIAN * cos(avg_lat * DEGREE_TO_RADIAN));
}


double lat_from_y(double y) {
    return y / DEGREE_TO_RADIAN;
}

/* Find euclidian distance between 2 points */
double euclidian(LatLon startPos, LatLon endPos) {
    std::pair<LatLon, LatLon> pair(startPos, endPos);
    return find_distance_between_two_points(pair);
}

/* Find manhattan distance between 2 points*/
double manhattan(LatLon startPos, LatLon endPos) {
    LatLon interim(endPos.lat(), startPos.lon());
    std::pair<LatLon, LatLon> start_to_interim(startPos, interim);
    std::pair<LatLon, LatLon> interim_to_end(interim, endPos);
    
    return find_distance_between_two_points(start_to_interim) + 
            find_distance_between_two_points(interim_to_end);
}

/* Find the common street segment of 2 intersections*/
std::vector<int> find_common_segment(int fromIndex, int toIndex, bool ignoreOneway) {
    std::vector<int> common_segment;
    std::vector<int> segment1 = inter_info[fromIndex].get_ss_id();
    std::vector<int> segment2 = inter_info[toIndex].get_ss_id();
 
    assert(!segment1.empty());
    assert(!segment2.empty());
    
    for (auto it = segment1.begin(); it != segment1.end(); ++it) {
        auto result = std::find(segment2.begin(), segment2.end(), *it);
        if (result != segment2.end()) {

            /* Check one way condition, ignore if it is one way in the opposite directions */
            if (!ignoreOneway) {
                if (ss_info[*it].get_one_way() && getInfoStreetSegment(*it).from != fromIndex && getInfoStreetSegment(*it).to != toIndex)
                    continue;
            }
            common_segment.push_back(*it);
        }
    }
    
    assert(!common_segment.empty());
    return common_segment;
}