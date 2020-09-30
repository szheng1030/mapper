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
#include <cmath>
#include <cctype>
#include <algorithm>
#include <unordered_set>
#include <string>

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/canvas.hpp"
#include "Node.h"

double avg_lat;
double min_lat, max_lat, max_lon, min_lon;

bool click_1 = false;
bool click_2 = false;
IntersectionIndex intersection_id_start;
IntersectionIndex intersection_id_end;

double map_height;

void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y){
    //Dehighlighting past intersections
    for (int i = 0; i < getNumIntersections(); i++)
        inter_info[i].set_highlight(false);
    
    LatLon pos = LatLon(lat_from_y(y), lon_from_x(x));
    int id = find_closest_intersection(pos);
    
    // first intersection
    if(!click_1){
        intersection_id_start = id;
        inter_info[intersection_id_start].set_highlight(true);
        
        click_1 = true;
        click_2 = false;
    }
    // second intersection
    else if(!click_2){
        intersection_id_end = id;     
                
        click_2 = true;
        click_1 = false;
        
        std::string directions = "";
        double street_length = 0;
        if(!walk_mode){
            PATH = find_path_between_intersections(intersection_id_start, intersection_id_end, TURN_PENALTY);
            
        
            for (int i = 0; i < getNumIntersections(); i++){
                inter_info[i].set_highlight(false);
            }
        
            inter_info[intersection_id_start].set_highlight(true);
            inter_info[intersection_id_end].set_highlight(true);
        
            std::string prev_name = getStreetName(getInfoStreetSegment(PATH[0]).streetID);
            directions += "Begin at " + getIntersectionName(intersection_id_start) + ".\n";
            directions += "\nDrive on " + prev_name + ".\n";
            for (int i = 0; i < PATH.size(); i++){
                 if (getStreetName(getInfoStreetSegment(PATH[i]).streetID) != prev_name){
                     directions += "\nContinue on " + prev_name + " for " + std::to_string(street_length) + "m.\n";
                     prev_name = getStreetName(getInfoStreetSegment(PATH[i]).streetID);
                     directions += "\nTurn onto " + prev_name + ".\n";
                     street_length = 0;
                 }
                 street_length += find_street_segment_length(PATH[i]);
            }
            directions += "\nContinue on " + prev_name + " for " + std::to_string(street_length) + "m.\n";
            directions += "\nYou have arrived at "+ getIntersectionName(intersection_id_end) + ".\n";
        }
        else{
            PATH_WALK_AND_DRIVE = find_path_with_walk_to_pick_up(intersection_id_start, intersection_id_end, TURN_PENALTY, atof(app->get_walk_speed().c_str()), atof(app->get_walk_time().c_str()));
            
            for (int i = 0; i < getNumIntersections(); i++){
                inter_info[i].set_highlight(false);
            }
        
            inter_info[intersection_id_start].set_highlight(true);
            inter_info[intersection_id_end].set_highlight(true);
          
            std::string prev_name;
          
            if (PATH_WALK_AND_DRIVE.first[0] != NULL){
              inter_info[getInfoStreetSegment(PATH_WALK_AND_DRIVE.first[PATH_WALK_AND_DRIVE.first.size() - 1]).to].set_highlight(true);
          
          
              prev_name = getStreetName(getInfoStreetSegment(PATH_WALK_AND_DRIVE.first[0]).streetID);
              directions += "Begin at " + getIntersectionName(intersection_id_start) + ".\n";
              directions += "\nWalk on " + prev_name + ".\n";
          
               for (int i = 0; i < PATH_WALK_AND_DRIVE.first.size(); i++){
                    if (getStreetName(getInfoStreetSegment(PATH_WALK_AND_DRIVE.first[i]).streetID) != prev_name){
                        directions += "\nContinue walking on " + prev_name + " for " + std::to_string(street_length) + "m.\n";
                        prev_name = getStreetName(getInfoStreetSegment(PATH_WALK_AND_DRIVE.first[i]).streetID);
                        directions += "\nTurn onto " + prev_name + ".\n";
                        street_length = 0;
                    }
                    street_length += find_street_segment_length(PATH_WALK_AND_DRIVE.first[i]);
                }
                directions += "\nContinue on " + prev_name + " for " + std::to_string(street_length) + "m.\n";
                directions += "\nAt " + getIntersectionName(getInfoStreetSegment(PATH_WALK_AND_DRIVE.first[PATH_WALK_AND_DRIVE.first.size() - 1]).to) + ", hop into the car.\n";
            }
            else{
              prev_name = getStreetName(getInfoStreetSegment(PATH_WALK_AND_DRIVE.second[0]).streetID);
              directions += "You don't have enough time and speed to get to a closer intersection!\n";
              directions += "\nBegin at " + getIntersectionName(intersection_id_start) + ".\n";
              directions += "\nDrive on " + prev_name + ".\n";
          }
          
          for (int i = 0; i < PATH_WALK_AND_DRIVE.second.size(); i++){
            if (getStreetName(getInfoStreetSegment(PATH_WALK_AND_DRIVE.second[i]).streetID) != prev_name){
                directions += "\nContinue on " + prev_name + " for " + std::to_string(street_length) + "m.\n";
                prev_name = getStreetName(getInfoStreetSegment(PATH_WALK_AND_DRIVE.second[i]).streetID);
                directions += "\nTurn onto " + prev_name + ".\n";
                street_length = 0;
            }
            street_length += find_street_segment_length(PATH_WALK_AND_DRIVE.second[i]);
          }
          directions += "\nContinue on " + prev_name + " for " + std::to_string(street_length) + "m.\n";
          directions += "\nYou have arrived at "+ getIntersectionName(intersection_id_end) + ".\n";
        }
        
        
        
        app->update_info(directions);
    }
    
    //Get coordinate of mouse click
    std::string clickLat = std::to_string(pos.lat());
    std::string clickLon = std::to_string(pos.lon());
    LatLon interPos = getIntersectionPosition(id);
    std::string lat = std::to_string(interPos.lat());
    std::string lon = std::to_string(interPos.lon());
    
    //std::cout << id << std::endl;
    
    //Set highlight status of intersection
    //inter_info[id].set_highlight(true);
    
    
    //app->refresh_drawing();
    //std::string click = "Closest intersection to \n(" + clickLat + ", " + clickLon + ")\n\n";
   // std::string info = getIntersectionName(id) + "\n" + "(" + lat + ", " + lon + ")";
    //app->update_info(click + info);
    app->refresh_drawing();
}

void draw_map() {
    //Default settings for map
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    ezgl::application application(settings);
    
    //Globals re-initializations
    map_height = 100;
    
    //Start application
    ezgl::rectangle initial_world({min_lon, min_lat}, {max_lon, max_lat});
    application.add_canvas("MainCanvas", draw_main_canvas, initial_world, ezgl::color(232, 232, 232));
    application.run(nullptr, act_on_mouse_click, nullptr, nullptr);
}

void reset_map(ezgl::application* ezgl_app){
    //Globals re-initializations
    map_height = 100;
    
    //Start application
    ezgl::rectangle initial_world({min_lon, min_lat}, {max_lon, max_lat});
    ezgl_app->change_canvas_world_coordinates("MainCanvas", initial_world);
    ezgl_app->refresh_drawing();
}

void draw_main_canvas (ezgl::renderer *g) {
    //Draw functions for elements in map
    draw_features(g);
    draw_street_segments(g);
    draw_intersections(g);  
    //draw_points_of_interest_logos(g);
    //draw_points_of_interest(g);
    //draw_street_names_and_one_way(g);
    
    if(!walk_mode){
        draw_path(PATH, g, false);
    }
    else{
        draw_path(PATH_WALK_AND_DRIVE.first, g, true);
        draw_path(PATH_WALK_AND_DRIVE.second, g, false);
    }
    
    //std::cout << "Map height: " << map_height << std::endl;
}

void draw_street_segments(ezgl::renderer *g) {
    g->set_line_cap(ezgl::line_cap::round);
    
    //Draw street segments from smallest -> largest
    //Iterate through containers that hold smaller street segments
    //Only if map is zoomed in enough to reduce runtime
    if (map_height < 0.0005)
        draw_small_street_segments(g);
    draw_med_street_segments(g);
    draw_large_street_segments(g); 
}

void draw_features(ezgl::renderer *g) {
    //Draw features from smallest -> largest
    //Iterate through containers that hold smaller features
    //Only if map is zoomed in enough to reduce runtime
    draw_exlarge_features(g);
    draw_large_features(g);
    draw_med_features(g);
    draw_small_features(g);
    if (map_height < 0.00025)
        draw_exsmall_features(g);
    
}

//Spadina Avenue and College Street
//Yonge Street and King Street East
//1 / 300

void draw_intersections(ezgl::renderer *g) {
     //Draw all intersections
    for (int i = 0; i < getNumIntersections(); i++) {
        double x = x_from_lon(inter_info[i].get_position().lon());
        double y = y_from_lat(inter_info[i].get_position().lat());
        
        //Special case for highlighted intersection
        if (inter_info[i].get_highlight()){
            g->set_color(ezgl::color(247, 153, 161));
            g->fill_arc({x, y}, set_highlighted_intersection_size(), 0, 360);
        }
        else {
            if (map_height < 0.0005) {
                g->set_color(ezgl::color(215, 215, 215));
                //g->fill_arc({x, y}, set_intersection_size(), 0, 360);
            }
        }
    }   
}

void draw_points_of_interest(ezgl::renderer *g){
    g->set_color(ezgl::color(105, 76, 6));
    
    //POI names only display if mapped is zoomed in enough
    if (map_height < 0.002) {
        for(int i = 0; i < getNumPointsOfInterest(); i++){
            
            //Draw POIs for the below classified places
            if( ( points_of_interest[i].get_type() == "restaurant" 
               || points_of_interest[i].get_type() == "bar" 
               || points_of_interest[i].get_type() == "dentist" 
               || points_of_interest[i].get_type() == "pharmacy"
               || points_of_interest[i].get_type() == "college"
               || points_of_interest[i].get_type() == "school"
               || points_of_interest[i].get_type() == "place_of_worship"
               || points_of_interest[i].get_type() == "cafe" )
               && map_height < 0.002){

                double x = points_of_interest[i].get_position().lon();
                double y = points_of_interest[i].get_position().lat();
                std::string name = points_of_interest[i].get_name();
                x = x_from_lon(x);
                y = y_from_lat(y);

                g->draw_text({x, y}, name, 0.001, 0.001);
            }
        }
    }
}

void draw_points_of_interest_logos(ezgl::renderer *g){
    
    if (map_height < 0.002) {
        ezgl::surface static *bar_ = g->load_png("libstreetmap/resources/Bar_resized.png");
        ezgl::surface static *restaurant_ = g->load_png("libstreetmap/resources/Restaurant_resized.png");
        ezgl::surface static *dentist_ = g->load_png("libstreetmap/resources/Dentist_resized.png");
        ezgl::surface static *pharmacy_ = g->load_png("libstreetmap/resources/Pharmacy_resized.png");
        ezgl::surface static *college_ = g->load_png("libstreetmap/resources/College_resized.png");
        ezgl::surface static *school_ = g->load_png("libstreetmap/resources/School_resized.png");
        ezgl::surface static *place_of_worship_ = g->load_png("libstreetmap/resources/Place_of_worship_resized.png");
        ezgl::surface static *cafe_ = g->load_png("libstreetmap/resources/Cafe_resized.png");

        for(int i = 0; i < getNumPointsOfInterest(); i++){

            if( ( points_of_interest[i].get_type() == "restaurant" 
               || points_of_interest[i].get_type() == "bar" 
               || points_of_interest[i].get_type() == "dentist" 
               || points_of_interest[i].get_type() == "pharmacy"
               || points_of_interest[i].get_type() == "college"
               || points_of_interest[i].get_type() == "school"
               || points_of_interest[i].get_type() == "place_of_worship"
               || points_of_interest[i].get_type() == "cafe" )
               && map_height < 0.002){

                double x = points_of_interest[i].get_position().lon();
                double y = points_of_interest[i].get_position().lat();
                //std::string name = points_of_interest[i].get_name();
                x = x_from_lon(x);
                y = y_from_lat(y);

                if(points_of_interest[i].get_type() == "restaurant" )
                    g->draw_surface(restaurant_, {x, y-0.000008});
                else if(points_of_interest[i].get_type() == "bar")
                    g->draw_surface(bar_, {x, y-0.000008});
                else if(points_of_interest[i].get_type() == "dentist")
                    g->draw_surface(dentist_, {x, y-0.000008});
                else if(points_of_interest[i].get_type() == "pharmacy")
                    g->draw_surface(pharmacy_, {x, y-0.000008});
                else if(points_of_interest[i].get_type() == "college")
                    g->draw_surface(college_, {x, y-0.000008});
                else if(points_of_interest[i].get_type() == "school")
                    g->draw_surface(school_, {x, y-0.000008});
                else if(points_of_interest[i].get_type() == "place_of_worship")
                    g->draw_surface(place_of_worship_, {x, y-0.000008});
                else if(points_of_interest[i].get_type() == "cafe")
                    g->draw_surface(cafe_, {x, y-0.000008});
            }
        }
    }
    // pictures:
    // https://commons.wikimedia.org/wiki/File:Fork_%26_knife.svg // restaurant
    // https://www.iconsdb.com/black-icons/bar-icon.html // bar
    // https://www.creativefabrica.com/product/dentist-icon-2/ // dentist
    // https://www.open-pharmacy-research.ca/cropped-open-site-icon-png/ // pharmacy
    // https://www.womenhelpingwomen.org/home/services/college-icon/ // college
    // http://www.iconarchive.com/show/ios7-icons-by-icons8/Science-School-icon.html // school
    // https://www.iconfinder.com/icons/1827591/christian_church_man_praying_place_of_worship_pray_religion_worship_icon // place_of_worship
}

void draw_street_names_and_one_way(ezgl::renderer *g){
    double angle = 0;
    double x_mid, y_mid;
    int street_name_spacing = 0;
    
    for(int i = 0; i < getNumStreetSegments(); i++){
        std::vector<LatLon> points = ss_info[i].get_points();
        //Find the mid-point of street segments
        //And angle between the start and end of street segments
        find_mid_and_angle(x_mid, y_mid, angle, points);
        
        //Display conditions
        
        if(points.size() > 1
           && street_name_spacing >= 7
           && display_street_name(ss_info[i].get_road_type())
           && ss_info[i].get_street_name() != "<unknown>"
           && ( ss_info[i].get_length() / points.size() ) > 10) {
            
            g->set_color(ezgl::color(50, 50,50));
            g->set_text_rotation(angle);
            g->set_font_size(13);
            g->draw_text({x_mid, y_mid}, ss_info[i].get_street_name(), ss_info[i].get_length(), 1);
            street_name_spacing = 0;
        }
        //Street name spacing used to reduce the number of times
        //A street name is drawn for a singular street
        else {
            street_name_spacing++;
        }
        
        //Draw all one-way arrows
        if (ss_info[i].get_one_way() && map_height < 0.01) {
            g->set_font_size(8);
            g->set_color(ezgl::color(215, 215, 215));
            set_one_way_color(ss_info[i].get_road_type(), g);
            g->set_text_rotation(angle);
            g->draw_text({x_mid, y_mid}, "►");
               
        }
    }
}

void find_mid_and_angle(double& x_mid, double& y_mid, double& angle, std::vector<LatLon> points) {
    //Retrieve points from LatLon vector
    //std::cout << "Points size: " << points.size() << std::endl;
    //std::cout << "Points index: " << points[0] << "   " << points[1] << std::endl;
    double x1 = x_from_lon(points[points.size()/2 -1].lon());
    double x2 = x_from_lon(points[points.size()/2].lon());
    double y1 = y_from_lat(points[points.size()/2 - 1].lat());
    double y2 = y_from_lat(points[points.size()/2].lat());
    
    //Parsing orientation of the 2 points to figure out
    //Midpoint and angle relation
    
    // Quadrant 1
    if(x1 <= x2 && y1 <= y2){
        x_mid = x1 + (x2 - x1) / 2;
        y_mid = y1 + (y2 - y1) / 2;
        
        angle = std::atan2( (y2-y1), (x2-x1) ) * (180 / M_PI);
        //std::cout << "Quadrant 1" << std::endl;
    }
    // Quadrant 2
    else if(x1 >= x2 && y1 <= y2){
        x_mid = x2 + (x1 - x2) / 2;
        y_mid = y1 + (y2 - y1) / 2;
        
        angle = 180 - std::atan2( (y2-y1), (x1-x2) ) * (180 / M_PI);
        //std::cout << "Quadrant 2" << std::endl;
    }
    // Quadrant 3
    else if(x1 >= x2 && y1 >= y2){
        x_mid = x2 + (x1 - x2) / 2;
        y_mid = y2 + (y1 - y2) / 2;
        
        angle = 180 + std::atan2( (y1-y2), (x1-x2) ) * (180 / M_PI);
        //std::cout << "Quadrant 3" << std::endl;
    }
    // Quadrant 4
    else if(x1 <= x2 && y1 >= y2){
        x_mid = x1 + (x2 - x1) / 2;
        y_mid = y2 + (y1 - y2) / 2;
        
        angle = -std::atan2( (y1-y2), (x2-x1) ) * (180 / M_PI);
        //std::cout << "Quadrant 4" << std::endl;
    }
}

void draw_arrow_between_intersections(double& x_mid, double& y_mid, double& angle, int int1, int int2){
    
    double x1 = x_from_lon( inter_info[int1].get_position().lon() );
    double x2 = x_from_lon( inter_info[int2].get_position().lon() );
    double y1 = y_from_lat( inter_info[int1].get_position().lat() );
    double y2 = y_from_lat( inter_info[int2].get_position().lat() );
    
    // Quadrant 1
    if(x1 <= x2 && y1 <= y2){
        x_mid = x1 + (x2 - x1) / 2;
        y_mid = y1 + (y2 - y1) / 2;
        
        angle = std::atan2( (y2-y1), (x2-x1) ) * (180 / M_PI);
    }
    // Quadrant 2
    else if(x1 >= x2 && y1 <= y2){
        x_mid = x2 + (x1 - x2) / 2;
        y_mid = y1 + (y2 - y1) / 2;
        
        angle = 180 - std::atan2( (y2-y1), (x1-x2) ) * (180 / M_PI);
    }
    // Quadrant 3
    else if(x1 >= x2 && y1 >= y2){
        x_mid = x2 + (x1 - x2) / 2;
        y_mid = y2 + (y1 - y2) / 2;
        
        angle = 180 + std::atan2( (y1-y2), (x1-x2) ) * (180 / M_PI);
    }
    // Quadrant 4
    else if(x1 <= x2 && y1 >= y2){
        x_mid = x1 + (x2 - x1) / 2;
        y_mid = y2 + (y1 - y2) / 2;
        
        angle = -std::atan2( (y1-y2), (x2-x1) ) * (180 / M_PI);
    }
}