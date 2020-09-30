/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m2helper.h"
#include "StreetData.h"
#include "Globals.h"


///////////////////////////////
///     STREET SEGMENTS     ///
///////////////////////////////

void draw_small_street_segments(ezgl::renderer *g) {
    for (int i = 0; i < small_ss_info.size(); i++) {
        set_street_width(ss_info[small_ss_info[i]].get_road_type(), g);
        set_street_color(ss_info[small_ss_info[i]].get_road_type(), g);


        for (int j = 0; j < ss_info[small_ss_info[i]].get_xy().size() - 1; j++)
            g->draw_line(ss_info[small_ss_info[i]].get_xy()[j], ss_info[small_ss_info[i]].get_xy()[j+1]);
    }
}

void draw_med_street_segments(ezgl::renderer *g) {
    for (int i = 0; i < med_ss_info.size(); i++) {
        set_street_width(ss_info[med_ss_info[i]].get_road_type(), g);
        set_street_color(ss_info[med_ss_info[i]].get_road_type(), g);


        for (int j = 0; j < ss_info[med_ss_info[i]].get_xy().size() - 1; j++)
            g->draw_line(ss_info[med_ss_info[i]].get_xy()[j], ss_info[med_ss_info[i]].get_xy()[j+1]);
    }
}

void draw_large_street_segments(ezgl::renderer *g) {
    for (int i = 0; i < large_ss_info.size(); i++) {
        set_street_width(ss_info[large_ss_info[i]].get_road_type(), g);
        set_street_color(ss_info[large_ss_info[i]].get_road_type(), g);


        for (int j = 0; j < ss_info[large_ss_info[i]].get_xy().size() - 1; j++)
            g->draw_line(ss_info[large_ss_info[i]].get_xy()[j], ss_info[large_ss_info[i]].get_xy()[j+1]);
    }
}


//////////////////////////////
///        FEATURES        ///
//////////////////////////////

void draw_exsmall_features(ezgl::renderer *g) {
    for (int i = 0; i < exsmall_feature_idx.size(); i++) {
        if ((feature_info[exsmall_feature_idx[i]].get_closed()) && (feature_info[exsmall_feature_idx[i]].get_xy().size() > 1)) {
            set_feature_color(feature_info[exsmall_feature_idx[i]].get_type(), g);       
            g->fill_poly(feature_info[exsmall_feature_idx[i]].get_xy());
        }
        else if (!feature_info[exsmall_feature_idx[i]].get_closed()) {
            for (int j = 0; j < feature_info[exsmall_feature_idx[i]].get_xy().size() - 1; j++) {
                g->set_line_width(1);
                g->draw_line(feature_info[exsmall_feature_idx[i]].get_xy()[j],feature_info[exsmall_feature_idx[i]].get_xy()[j+1]);
            }
        }
    }
}

void draw_small_features(ezgl::renderer *g) {
    for (int i = 0; i < small_feature_idx.size(); i++) {
        if ((feature_info[small_feature_idx[i]].get_closed()) && (feature_info[small_feature_idx[i]].get_xy().size() > 1)) {
            set_feature_color(feature_info[small_feature_idx[i]].get_type(), g);       
            g->fill_poly(feature_info[small_feature_idx[i]].get_xy());
        }
        else if (!feature_info[small_feature_idx[i]].get_closed()) {
            for (int j = 0; j < feature_info[small_feature_idx[i]].get_xy().size() - 1; j++) {
                g->set_line_width(1);
                g->draw_line(feature_info[small_feature_idx[i]].get_xy()[j],feature_info[small_feature_idx[i]].get_xy()[j+1]);
            }
        }
    }
}

void draw_med_features(ezgl::renderer *g) {
    for (int i = 0; i < med_feature_idx.size(); i++) {
        if ((feature_info[med_feature_idx[i]].get_closed()) && (feature_info[med_feature_idx[i]].get_xy().size() > 1)) {
            set_feature_color(feature_info[med_feature_idx[i]].get_type(), g);        
            g->fill_poly(feature_info[med_feature_idx[i]].get_xy());
        }
        else if (!feature_info[med_feature_idx[i]].get_closed()) {
            for (int j = 0; j < feature_info[med_feature_idx[i]].get_xy().size() - 1; j++) {
                g->set_line_width(1);
                g->draw_line(feature_info[med_feature_idx[i]].get_xy()[j],feature_info[med_feature_idx[i]].get_xy()[j+1]);
            }
        }
    }
}

void draw_large_features(ezgl::renderer *g) {
    for (int i = 0; i < large_feature_idx.size(); i++) {
        if ((feature_info[large_feature_idx[i]].get_closed()) && (feature_info[large_feature_idx[i]].get_xy().size() > 1)) {
            set_feature_color(feature_info[large_feature_idx[i]].get_type(), g);        
            g->fill_poly(feature_info[large_feature_idx[i]].get_xy());
        }
        else if (!feature_info[large_feature_idx[i]].get_closed()) {
            for (int j = 0; j < feature_info[large_feature_idx[i]].get_xy().size() - 1; j++) {
                g->set_line_width(1);
                g->draw_line(feature_info[large_feature_idx[i]].get_xy()[j],feature_info[large_feature_idx[i]].get_xy()[j+1]);
            }
        }
    }
}

void draw_exlarge_features(ezgl::renderer *g) {
    for (int i = 0; i < exlarge_feature_idx.size(); i++) {
        if ((feature_info[exlarge_feature_idx[i]].get_closed()) && (feature_info[exlarge_feature_idx[i]].get_xy().size() > 1)) {
            set_feature_color(feature_info[exlarge_feature_idx[i]].get_type(), g);        
            g->fill_poly(feature_info[exlarge_feature_idx[i]].get_xy());
        }
        else if (!feature_info[exlarge_feature_idx[i]].get_closed()) {
            for (int j = 0; j < feature_info[exlarge_feature_idx[i]].get_xy().size() - 1; j++) {
                g->set_line_width(1);
                g->draw_line(feature_info[exlarge_feature_idx[i]].get_xy()[j],feature_info[exlarge_feature_idx[i]].get_xy()[j+1]);
            }
        }
    }
}


///////////////////////////////
///      INTERSECTIONS      ///
///////////////////////////////

void draw_normal_intersections(ezgl::renderer *g) {
    for (int i = 0; i < getNumIntersections(); i++){
        double x = x_from_lon(inter_info[i].get_position().lon());
        double y = inter_info[i].get_position().lat();

        g->set_color(ezgl::color(213, 216, 219));
        g->fill_arc({x, y}, set_intersection_size(), 0, 360);
    }
}

void draw_highlighted_intersection(int last_int, ezgl::renderer *g) {
    double x = x_from_lon(inter_info[last_int].get_position().lon());
    double y = inter_info[last_int].get_position().lat();
    g->set_color(ezgl::color(247, 153, 161));
    g->fill_arc({x, y}, set_highlighted_intersection_size(), 0, 360);
}
