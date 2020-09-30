/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m2helper.h"
#include "StreetData.h"

//Set color of feature based on type of feature
void set_feature_color(FeatureType type, ezgl::renderer *g) {
    switch (type) {
        case Park:
        case Golfcourse:
        case Greenspace:
            g->set_color(ezgl::color(195, 236, 178));
            break;
        case Beach:
            g->set_color(ezgl::color(246, 207, 101));
            break;
        case Stream:
        case River:
        case Lake:
            g->set_color(ezgl::color(170, 218, 255));
            break;
        case Island:
            g->set_color(ezgl::color(232, 232, 232));
            break;
        case Building:
            g->set_color(ezgl::color(213, 216, 219));
            break;
        default:
            g->set_color(ezgl::color(232, 232, 232));
            break;
    }
}


//Set line thickness of streets based on their classification
void set_street_width(Roads road_type, ezgl::renderer *g) {
    switch(road_type) {
        case Motorway:
        case Trunk:
        case Primary:
            if (map_height > 0.004) g->set_line_width(1);
            else if (map_height > 0.002) g->set_line_width(2);
            else if (map_height > 0.001) g->set_line_width(3);
            else if (map_height > 0.0005) g->set_line_width(4);
            else if (map_height > 0.00025) g->set_line_width(6);
            else if (map_height > 0.00012) g->set_line_width(8);
            else if (map_height > 0.00006) g->set_line_width(15);
            else  g->set_line_width(20);
            break;
        case Secondary:
        case Tertiary:
            if (map_height > 0.002) g->set_line_width(1);
            else if (map_height > 0.001) g->set_line_width(2);
            else if (map_height > 0.0005) g->set_line_width(3);
            else if (map_height > 0.00012) g->set_line_width(5);
            else if (map_height > 0.00006) g->set_line_width(10);
            else g->set_line_width(17);
            break;
        case Residential:
            if (map_height > 0.00012) g->set_line_width(2);
            else if (map_height > 0.00006) g->set_line_width(6);
            else g->set_line_width(13);
            break;
        case Unclassified:
        case Other:
            if (map_height > 0.00012) g->set_line_width(1);
            else if (map_height > 0.00006) g->set_line_width(5);
            else g->set_line_width(10);
            break;
        default:
            g->set_line_width(0);
            break;
    }
}

//Set color of street based on their classification
void set_street_color(Roads road_type, ezgl::renderer *g) {
    switch(road_type) {
        case Motorway:
        case Trunk:
        case Primary:
            g->set_color(ezgl::color(255, 235, 161));
            break;
        case Secondary:
        case Tertiary:
        case Unclassified:
        case Residential:
        case Other:
            g->set_color(ezgl::WHITE);
            break;
        default:
            g->set_color(ezgl::WHITE);
            break;
    }
}

//Set color of one way arrows based on what type of street
//To create sufficient contrast
void set_one_way_color(Roads road_type, ezgl::renderer *g) {
    switch(road_type) {
        case Motorway:
        case Trunk:
        case Primary:
        case Secondary:
            g->set_color(ezgl::WHITE);
            break;
        case Tertiary:
        case Unclassified:
        case Residential:
        case Other:
            g->set_color(ezgl::color(181, 183, 186));
            break;
        default:
            g->set_color(ezgl::color(181, 183, 186));
            break;
    }
}

//Set size of highlighted intersection circle based on world to screen ratio
double set_highlighted_intersection_size() {
    if (map_height > 0.005)
        return 0.000055;
    else if (map_height > 0.003)
        return 0.000035;
    else if (map_height > 0.001)
        return 0.00002;
    else if (map_height > 0.0005)
        return 0.00001;
    else if (map_height > 0.00025)
        return 0.000006;
    else if (map_height > 0.00012)
        return 0.0000035;
    else if (map_height > 0.00006)
        return 0.0000025;
    else if (map_height > 0.00003)
        return 0.0000013;
    else
        return 0.000001;
}

//Set size of non-highlighted intersections based on world to screen ratio
double set_intersection_size() {
    if (map_height > 0.00025)
        return 0.0000025;
    else if (map_height > 0.00012)
        return 0.0000018;
    else if (map_height > 0.00006)
        return 0.0000014;
    else if (map_height > 0.00003)
        return 0.000001;
    else
        return 0.0000003;
}

//Set if street name is displayed based of street type classification
//And world to screen ratio
bool display_street_name(Roads road_type) {

    switch(road_type) {
        case Motorway:
            return (map_height < 0.0004);
            break;
        case Trunk:
            return (map_height < 0.0004);
            break;
        case Primary:
            return (map_height < 0.0004);
            break;
        case Secondary:
            return (map_height < 0.0002);
            break;
        case Tertiary:
            return (map_height < 0.0001);
            break;
        case Residential:
            return (map_height < 0.00005);
            break;
        case Other:
        case Unclassified:
        default:
            return false;
            break;
    }
}