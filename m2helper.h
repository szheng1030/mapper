/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m2helper.h
 * Author: sharapo1
 *
 * Created on February 18, 2020, 1:33 PM
 */

#pragma once

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

//Graphical interface functions
void draw_main_canvas (ezgl::renderer *g);
void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y);
void test_button(GtkWidget *widget, ezgl::application *application);
void reset_map(ezgl::application* ezgl_app);

//LatLon to coordinate conversion functions
double x_from_lon(double lon);
double y_from_lat(double lat);
double lon_from_x(double x);
double lat_from_y(double y);

//Draw intersections functions
void draw_intersections(ezgl::renderer *g);
void draw_normal_intersections(ezgl::renderer *g);
void draw_highlighted_intersection(int last_int, ezgl::renderer *g);

//Draw street segments functions
void draw_street_segments(ezgl::renderer *g);
void draw_small_street_segments(ezgl::renderer *g);
void draw_med_street_segments(ezgl::renderer *g);
void draw_large_street_segments(ezgl::renderer *g);

//Draw feature functions
void draw_features(ezgl::renderer *g);
void draw_exsmall_features(ezgl::renderer *g);
void draw_small_features(ezgl::renderer *g);
void draw_med_features(ezgl::renderer *g);
void draw_large_features(ezgl::renderer *g);
void draw_exlarge_features(ezgl::renderer *g);

//Draw street overlay functions (names, oneway, etc.)
void draw_intersection_names(ezgl::renderer *g);
void draw_street_names_and_one_way(ezgl::renderer *g);

//Draw points of interest functions
void draw_points_of_interest(ezgl::renderer *g);
void draw_points_of_interest_logos(ezgl::renderer *g);

//Set visualization colours
void set_feature_color(FeatureType type, ezgl::renderer *g);
void set_street_color(Roads road_type, ezgl::renderer *g);
void set_one_way_color(Roads road_type, ezgl::renderer *g);

//Set display limitations based on world to screen ratio
bool display_street_name(Roads road_type);
void find_mid_and_angle(double& x_mid, double& y_mid, double& angle, std::vector<LatLon> points);
void draw_arrow_between_intersections(double& x_mid, double& y_mid, double& angle, int int1, int int2);

//Set element display size based on world to screen ratio
void set_street_width(Roads road_type, ezgl::renderer *g);
double set_highlighted_intersection_size();
double set_intersection_size();
