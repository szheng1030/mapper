/* 
 * File:   m1helper.h
 * Author: zhengsa3
 *
 * Created on March 7, 2020, 5:30 AM
 */

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"

#include "m1helper.h"
#include "m2helper.h"
#include "StreetData.h"
#include "Globals.h"

//////////////////////////
//     CONSTRUCT        //
//     INTERSECTION     //
//////////////////////////
void construct_intersection_info() {
    for (int intersection = 0; intersection < getNumIntersections(); intersection++) {
        for (int i = 0; i < getIntersectionStreetSegmentCount(intersection); i++) {
 
            //For find_street_segment_of_intersection
            //With ss_id, can use ss_id to find_street_names_of_intersection
            int segment = getIntersectionStreetSegment(intersection, i);
            inter_info[intersection].ss_id.push_back(segment);
 
            //Finding intersections of streets
            ss_info[segment].street_id = getInfoStreetSegment(segment).streetID;
            street_info[ss_info[segment].street_id].inter_id.insert(intersection);
        }
        //Finding position of each intersection
        inter_info[intersection].position = getIntersectionPosition(intersection);
        
        //Update max/min lat/lons with newest value from intersections
        double x = inter_info[intersection].get_position().lon();
        double y = inter_info[intersection].get_position().lat();
        
        max_lat = std::max(max_lat, y);
        min_lat = std::min(min_lat, y);
        max_lon = std::max(max_lon, x);
        min_lon = std::min(min_lon, x);
    }
}


////////////////////////////
//     CONSTRUCT          //
//     STREET SEGMENTS    //
////////////////////////////
void construct_street_segment_info() {
    for (int segment = 0; segment < getNumStreetSegments(); segment++) {
        InfoStreetSegment street_segment = getInfoStreetSegment(segment);
 
        //Direct data from API
        ss_info[segment].speed_limit = street_segment.speedLimit / KMH_TO_MS;
        ss_info[segment].street_name = getStreetName(street_segment.streetID);
        ss_info[segment].way_id = street_segment.wayOSMID;
        ss_info[segment].one_way = street_segment.oneWay;
 
        //Create vector to store all points in LatLon coords
        ss_info[segment].points.push_back(getIntersectionPosition(street_segment.from));
        for (int i = 0; i < street_segment.curvePointCount; i++)
            ss_info[segment].points.push_back(getStreetSegmentCurvePoint(i, segment));
        ss_info[segment].points.push_back(getIntersectionPosition(street_segment.to));
        
        //Create vector to store all points in XY coords 
        for (int i = 0; i < ss_info[segment].points.size(); i++) {
            ezgl::point2d xy(x_from_lon(ss_info[segment].points[i].lon()), 
                    y_from_lat(ss_info[segment].points[i].lat()));
            ss_info[segment].xy.push_back(xy);
        }
        
        //Calculate length of street segment from distance between each point
        for (int i = 0; i < ss_info[segment].points.size() - 1; i++) {
            std::pair<LatLon, LatLon> pair(ss_info[segment].points[i], ss_info[segment].points[i+1]);
            ss_info[segment].length += find_distance_between_two_points(pair);
        }
        
        //Calculate travel time = distance / travel time
        ss_info[segment].travel_time = ss_info[segment].speed_limit != 0 ?
            (ss_info[segment].length / ss_info[segment].speed_limit) : 0;
 
        //THIS IS THE FUCKING REASON WHY OUR RUNTIME IS SHIT//
        /*//Create vector to store all street segments that make up parent street
        bool ss_exists = false;
        //Add street segments to vector for each street, checking duplicates
        for (auto it = street_info[street_segment.streetID].ss_id.begin(); it != street_info[street_segment.streetID].ss_id.end(); it++) {
            if (*it == segment) {
                ss_exists = true;
                break;
            }
        }
        if (!ss_exists) {
            street_info[street_segment.streetID].ss_id.push_back(segment);
            street_info[street_segment.streetID].length += ss_info[segment].length;
        }*/
        
        street_info[street_segment.streetID].ss_id.push_back(segment);
        street_info[street_segment.streetID].length += ss_info[segment].length;
        
        //For find_street_ids_from_partial_street_name
        street_info[ss_info[segment].street_id].name = ss_info[segment].street_name;
        std::string name = ss_info[segment].street_name;
        //Calculating hash key from char[0] * char[1]
        int name_index = name[0] * name[1];
        if (name_index < 0) {
            name_index = -name_index;
        }
        //Remove all whitespaces in street name for comparison
        name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
        street_info[ss_info[segment].street_id].db_name = name;
        //Add street names into unordered_set based on key
        street_name_db[name_index].insert(street_segment.streetID);
        
        
        //Find road type
        const OSMWay& e = (*OSMWayTable.find(ss_info[segment].get_way_id())).second;
        int count = getTagCount(&e);
        
        for (int i = 0; i < count; i++) {
            std::pair<std::string, std::string> tags = getTagPair(&e, i);
            
            if(tags.first == "highway"){
                if ((tags.second == "motorway") || (tags.second == "motorway_link")) {
                    ss_info[segment].road_type = Roads::Motorway;
                    large_ss_info.push_back(segment);
                }
                else if ((tags.second == "trunk") || (tags.second == "trunk_link")) {
                    ss_info[segment].road_type = Roads::Trunk;
                    large_ss_info.push_back(segment);
                }
                else if ((tags.second == "primary") || (tags.second == "primary_link")) {
                    ss_info[segment].road_type = Roads::Primary;
                    large_ss_info.push_back(segment);
                }
                else if ((tags.second == "secondary") || (tags.second == "secondary_link")) {
                    ss_info[segment].road_type = Roads::Secondary;
                    med_ss_info.push_back(segment);
                }
                else if ((tags.second == "tertiary") || (tags.second == "tertiary_link")) {
                    ss_info[segment].road_type = Roads::Tertiary;
                    med_ss_info.push_back(segment);
                }
                else if (tags.second == "unclassified") {
                    ss_info[segment].road_type = Roads::Unclassified;
                    small_ss_info.push_back(segment);
                }
                else if (tags.second == "residential") {
                    ss_info[segment].road_type = Roads::Residential;
                    small_ss_info.push_back(segment);
                }
                else {
                    ss_info[segment].road_type = Roads::Other;
                    small_ss_info.push_back(segment);
                }
            }
        }
    }
}


///////////////////////
//     CONSTRUCT     //
//     FEATURE       //
///////////////////////
void construct_feature_info() {
    for (int feature = 0; feature < getNumFeatures(); feature++) {
        
        //Direct data from API
        feature_info[feature].type = getFeatureType(feature);
        feature_info[feature].pointCount = getFeaturePointCount(feature);
        feature_info[feature].id = getFeatureOSMID(feature);
        
        //Create vector to store all points in LatLon coords
        for (int i = 0; i < feature_info[feature].pointCount; i++) {
            feature_info[feature].points.push_back(getFeaturePoint(i, feature));
        }
        
        //Create vector to store all points in XY coords 
        for (int i = 0; i < feature_info[feature].points.size(); i++) {
            ezgl::point2d xy(x_from_lon(feature_info[feature].points[i].lon()), 
                    y_from_lat(feature_info[feature].points[i].lat()));
            feature_info[feature].xy.push_back(xy);
        }
        
        //Check if feature is closed or open
        if ((feature_info[feature].points[0].lat() == feature_info[feature].points[feature_info[feature].pointCount - 1].lat()) 
               && (feature_info[feature].points[0].lon() == feature_info[feature].points[feature_info[feature].pointCount - 1].lon())) {
            feature_info[feature].closed = true;

            //Shoelace formula for area
            //A = 1/2 [(x1 * y2 + x2 * y3 + ...) - (y1 * x2 + y2 * x3 + y3 * x4 + ...)]
            double area = 0.0;
            for (int i = 0; i < feature_info[feature].xy.size() - 1; i++)
                area += feature_info[feature].xy[i].x * feature_info[feature].xy[i+1].y - feature_info[feature].xy[i].y * feature_info[feature].xy[i+1].x;
            area = 0.5 * area * EARTH_RADIUS_METERS * EARTH_RADIUS_METERS;
            feature_info[feature].area = area < 0 ? -area : area;
            
            
            
            //NEEDS RECLASSIFICATION !!
            //Feature size classification for drawing
            if (feature_info[feature].type == FeatureType::Lake && feature_info[feature].area >= 500000.0)
                exlarge_feature_idx.push_back(feature);
            else if (feature_info[feature].type == FeatureType::Island)
                large_feature_idx.push_back(feature);
            else if (feature_info[feature].type == FeatureType::Golfcourse)
                med_feature_idx.push_back(feature);
            else if (feature_info[feature].type == FeatureType::Park)
                med_feature_idx.push_back(feature);
            else if (feature_info[feature].type == FeatureType::Beach)
                med_feature_idx.push_back(feature);
            else if (feature_info[feature].type == FeatureType::Greenspace)
                med_feature_idx.push_back(feature);
            else if (feature_info[feature].type == FeatureType::Lake)
                small_feature_idx.push_back(feature);
            else if (feature_info[feature].type == FeatureType::River)
                small_feature_idx.push_back(feature);
            else if (feature_info[feature].type == FeatureType::Stream)
                small_feature_idx.push_back(feature);
            else if (feature_info[feature].type == FeatureType::Building)
                exsmall_feature_idx.push_back(feature);
            else 
                exsmall_feature_idx.push_back(feature);
        }
        else 
            feature_info[feature].closed = false;
    }
}


////////////////////////
//     CONSTRUCT      //
//     OSM TABLES     //
////////////////////////
void create_OSM_tables() {
    //OSMNode Look-Up Table
    for (int i = 0; i < getNumberOfNodes(); i++) {
        OSMNodeTable.insert(std::make_pair(getNodeByIndex(i)->id() , *getNodeByIndex(i)));
    }
    //OSMWay Look-Up Table
    for (int i = 0; i < getNumberOfWays(); i++) {
        OSMWayTable.insert(std::make_pair(getWayByIndex(i)->id() , *getWayByIndex(i)));
    }
}


///////////////////////////////
//     CONSTRCUT             //
//     POINT OF INTERESTS    //
///////////////////////////////
void construct_pointofinterest_info() {
    for(int i = 0; i < getNumPointsOfInterest(); i++){
        
        points_of_interest[i].type = getPointOfInterestType(i);
        points_of_interest[i].name = getPointOfInterestName(i);
        points_of_interest[i].pos = getPointOfInterestPosition(i);
        points_of_interest[i].id = getPointOfInterestOSMNodeID(i);
        
        const OSMNode& e = (*OSMNodeTable.find(points_of_interest[i].id)).second;
        int count = getTagCount(&e);
        
        for(int j = 0; j < count; j++){
            std::pair<std::string, std::string> tags = getTagPair(&e, j);
        }
    }
}


//Initializes values for world bounds
//This is done once per map to establish bounds and average latitude
void world_bounds_initialization() {
    max_lat = getIntersectionPosition(0).lat();
    min_lat = max_lat;
    max_lon = getIntersectionPosition(0).lon();
    min_lon = max_lon;
}


//Calculate finalized values for world bounds
void world_bounds_finalization() {
    max_lon = x_from_lon(max_lon);
    min_lon = x_from_lon(min_lon);
    max_lat = y_from_lat(max_lat);
    min_lat = y_from_lat(min_lat);
    avg_lat = (max_lat + min_lat) / 2;
}