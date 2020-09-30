/*
 * Copyright 2020 University of Toronto
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors: Mario Badr, Sameh Attia and Tanner Young-Schultz
 */

#include "ezgl/callback.hpp"
#include "m2helper.h"
namespace ezgl {

// File wide static variables to track whether the middle mouse
// button is currently pressed AND the old x and y positions of the mouse pointer
bool left_mouse_button_pressed = false;
int last_panning_event_time = 0;
double prev_x = 0, prev_y = 0;

gboolean press_key(GtkWidget *, GdkEventKey *event, gpointer data)
{
  auto application = static_cast<ezgl::application *>(data);

  // Call the user-defined key press callback if defined
  if(application->key_press_callback != nullptr) {
    // see: https://developer.gnome.org/gdk3/stable/gdk3-Keyboard-Handling.html
    application->key_press_callback(application, event, gdk_keyval_name(event->keyval));
  }

  return FALSE; // propagate the event
}

gboolean press_mouse(GtkWidget *, GdkEventButton *event, gpointer data)
{
  auto application = static_cast<ezgl::application *>(data);
  if(event->type == GDK_BUTTON_PRESS) {

    // Check for Left mouse press to support dragging
    if(event->button == 1) {
      left_mouse_button_pressed = true;
      prev_x = event->x;
      prev_y = event->y;
    }

    // Call the user-defined mouse press callback if defined
    else if(application->mouse_press_callback != nullptr) {
      ezgl::point2d const widget_coordinates(event->x, event->y);

      std::string main_canvas_id = application->get_main_canvas_id();
      ezgl::canvas *canvas = application->get_canvas(main_canvas_id);

      ezgl::point2d const world = canvas->get_camera().widget_to_world(widget_coordinates);
      application->mouse_press_callback(application, event, world.x, world.y);
    }
  }

  return TRUE; // consume the event
}

gboolean release_mouse(GtkWidget *, GdkEventButton *event, gpointer )
{
  if(event->type == GDK_BUTTON_RELEASE) {
    // Check for Middle mouse release to support dragging
    if(event->button == 1) {
      left_mouse_button_pressed = false;
    }
  }

  return TRUE; // consume the event
}

gboolean move_mouse(GtkWidget *, GdkEventButton *event, gpointer data)
{
  auto application = static_cast<ezgl::application *>(data);

  if(event->type == GDK_MOTION_NOTIFY) {

    // Check if the middle mouse is pressed to support dragging
    if(left_mouse_button_pressed) {
      // drop this panning event if we have just served another one
      if(gtk_get_current_event_time() - last_panning_event_time < 100)
        return true;

      last_panning_event_time = gtk_get_current_event_time();

      GdkEventMotion *motion_event = (GdkEventMotion *)event;

      std::string main_canvas_id = application->get_main_canvas_id();
      auto canvas = application->get_canvas(main_canvas_id);

      point2d curr_trans = canvas->get_camera().widget_to_world({motion_event->x, motion_event->y});
      point2d prev_trans = canvas->get_camera().widget_to_world({prev_x, prev_y});

      double dx = curr_trans.x - prev_trans.x;
      double dy = curr_trans.y - prev_trans.y;

      prev_x = motion_event->x;
      prev_y = motion_event->y;

      // Flip the delta x to avoid inverted dragging
      translate(canvas, -dx, -dy);
    }
    // Else call the user-defined mouse move callback if defined
    else if(application->mouse_move_callback != nullptr) {
      ezgl::point2d const widget_coordinates(event->x, event->y);

      std::string main_canvas_id = application->get_main_canvas_id();
      ezgl::canvas *canvas = application->get_canvas(main_canvas_id);

      ezgl::point2d const world = canvas->get_camera().widget_to_world(widget_coordinates);
      application->mouse_move_callback(application, event, world.x, world.y);
    }
  }

  return TRUE; // consume the event
}

gboolean scroll_mouse(GtkWidget *, GdkEvent *event, gpointer data)
{

  if(event->type == GDK_SCROLL) {
    auto application = static_cast<ezgl::application *>(data);

    std::string main_canvas_id = application->get_main_canvas_id();
    auto canvas = application->get_canvas(main_canvas_id);

    GdkEventScroll *scroll_event = (GdkEventScroll *)event;

    ezgl::point2d scroll_point(scroll_event->x, scroll_event->y);

    if(scroll_event->direction == GDK_SCROLL_UP) {
        // Zoom in at the scroll point
        ezgl::zoom_in(canvas, scroll_point, 2.0);
    } else if(scroll_event->direction == GDK_SCROLL_DOWN) {
        // Zoom out at the scroll point
        ezgl::zoom_out(canvas, scroll_point, 2.0);
    } else if(scroll_event->direction == GDK_SCROLL_SMOOTH) {
      // Doesn't seem to be happening
    } // NOTE: We ignore scroll GDK_SCROLL_LEFT and GDK_SCROLL_RIGHT
  }
  return TRUE;
}

gboolean press_zoom_fit(GtkWidget *, gpointer data)
{

  auto application = static_cast<ezgl::application *>(data);

  std::string main_canvas_id = application->get_main_canvas_id();
  auto canvas = application->get_canvas(main_canvas_id);

  ezgl::zoom_fit(canvas, canvas->get_camera().get_initial_world());

  return TRUE;
}

gboolean press_zoom_in(GtkWidget *, gpointer data)
{
        auto application = static_cast<ezgl::application *>(data);

        std::string main_canvas_id = application->get_main_canvas_id();
        auto canvas = application->get_canvas(main_canvas_id);

        ezgl::zoom_in(canvas, 2.0);
    return TRUE;
}

gboolean press_zoom_out(GtkWidget *, gpointer data)
{
        auto application = static_cast<ezgl::application *>(data);

        std::string main_canvas_id = application->get_main_canvas_id();
        auto canvas = application->get_canvas(main_canvas_id);

        ezgl::zoom_out(canvas, 2.0);
    return TRUE;
}

//find button to do the search for the intersection of two inputted streets in the search bars
gboolean press_find(GtkWidget *, gpointer data)
{
  auto ezgl_app = static_cast<ezgl::application *>(data);
  
  //get the user inputted strings from the search bars
  //std::string firstName = ezgl_app->get_first_street();
  //std::string secondName = ezgl_app->get_second_street();
  
  std::string and_string = " and ";
  //Extract the two street names from the input string
  std::string firstText = ezgl_app->get_first_street();
  int and_pos = firstText.find(and_string);
  std::string inter1Street1 = firstText.substr(0, and_pos);
  std::string inter1Street2 = firstText;
  inter1Street2.erase(0, and_pos + and_string.length());
  
  //get_error_message catches any errors in the inputs, if there are none returns success and updates the intersection list
  std::vector<int> intersection1;
  std::string info = get_error_message(inter1Street1, inter1Street2, &intersection1);
  ezgl_app->update_info(info);
  
  //same thing for the second intersection
  std::string secondText = ezgl_app->get_second_street();
  and_pos = secondText.find(and_string);
  std::string inter2Street1 = secondText.substr(0, and_pos);
  std::string inter2Street2 = secondText;
  inter2Street2.erase(0, and_pos + and_string.length());
  
  //only attempt to find it if the first intersection was found
  std::vector<int> intersection2;
  if (info == "success!"){
      info = get_error_message(inter2Street1, inter2Street2, &intersection2);
      ezgl_app->update_info(info);
  }
  
  //If walk mode is enabled by the toggle, get the walk speed and time, but make sure they are valid doubles!
  std::string walk_speed, walk_time;
  double speed = 0, time = 0;
  if (info == "success!" && walk_mode){
      walk_speed = ezgl_app->get_walk_speed();
      walk_time = ezgl_app->get_walk_time();
      bool valid = (walk_speed.find_first_not_of(".0123456789") == std::string::npos)
        && (walk_time.find_first_not_of(".0123456789") == std::string::npos);
      if (!valid){
          info = "Error: please only input numbers as the walk time and speed";
          ezgl_app->update_info(info);
      }
      else{
          speed = atof(walk_speed.c_str());
          time = atof(walk_time.c_str());
      }
  }
  

  std::string directions = "";
  double street_length = 0;
  if (info == "success!"){
      if (!walk_mode){
        PATH = find_path_between_intersections(intersection1[0], intersection2[0], TURN_PENALTY);
        
        for (int i = 0; i < getNumIntersections(); i++){
            inter_info[i].set_highlight(false);
        }
        
        inter_info[intersection1[0]].set_highlight(true);
        inter_info[intersection2[0]].set_highlight(true);
        
        std::string prev_name = getStreetName(getInfoStreetSegment(PATH[0]).streetID);
        directions += "Begin at " + firstText + ".\n";
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
        directions += "\nYou have arrived at "+ secondText + ".\n";
      }
      else{
          PATH_WALK_AND_DRIVE = find_path_with_walk_to_pick_up(intersection1[0], intersection2[0], TURN_PENALTY, speed, time);
          
          
          for (int i = 0; i < getNumIntersections(); i++){
            inter_info[i].set_highlight(false);
          }
        
          inter_info[intersection1[0]].set_highlight(true);
          inter_info[intersection2[0]].set_highlight(true);
          
          std::string prev_name;

          if (PATH_WALK_AND_DRIVE.first[0] != NULL){
              inter_info[getInfoStreetSegment(PATH_WALK_AND_DRIVE.first[PATH_WALK_AND_DRIVE.first.size() - 1]).to].set_highlight(true);
              prev_name = getStreetName(getInfoStreetSegment(PATH_WALK_AND_DRIVE.first[0]).streetID);
              directions += "Begin at " + firstText + ".\n";
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
              directions += "\nBegin at " + firstText + ".\n";
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
          directions += "\nYou have arrived at "+ secondText + ".\n";
      }
  }
  
  if (info == "success!"){
      ezgl_app->update_info(directions);
  }
  
  ezgl_app->refresh_drawing();
  
  return TRUE;
}
  
std::string get_error_message(std::string firstName, std::string secondName, std::vector<int> *ans){
  
  //use partial street name to get the corresponding streets for the inputs
  std::vector<int> firstIds = find_street_ids_from_partial_street_name(firstName);
  std::vector<int> secondIds = find_street_ids_from_partial_street_name(secondName);

  //write errors to text field to notify users of the problems with their search fields, and return true to exit function
  //notify user if they left one or both of the search bars empty
  if (firstName == "" || secondName == ""){
      //ezgl_app->update_info("Error: please input a valid street name in each search bar");
      return "Error: please input two valid street names in each search bar like this: \"Street1 and Street2\"";
  }
  //notify user if their input doesn't match any streets
  if (firstIds.size() <= 0){
      //ezgl_app->update_info("Error: no street name matches \"" + firstName + "\"");
      return "Error: no street name matches \"" + firstName + "\"";
  }
  if (secondIds.size() <= 0){
      //ezgl_app->update_info("Error: no street name matches \"" + secondName + "\"");
      return "Error: no street name matches \"" + secondName + "\"";
  }
  
  //Some streets have the same name, or have versions with extra words like Bloor St vs Bloor St West
  //Have a vector containing all of the relevant results
  std::vector<int> firstIdsAns;
  std::vector<int> secondIdsAns;

  bool firstSame = true;
  bool secondSame = true;
  
  //check if all the results are actually the same street
  for (int i = 1; i < firstIds.size(); i++){
      if (getStreetName(firstIds[i]) != getStreetName(firstIds[i - 1])){
          firstSame = false;
          break;
      }
  }
  ///if so add to answer vector
  if (firstSame){
      for (int i = 0; i < firstIds.size(); i++){
          firstIdsAns.push_back(firstIds[i]);
      }
  }
  //otherwise check if any results exactly match the input, and then add those to result vector
  else{
      for (int i = 0; i < firstIds.size(); i++){
          if (getStreetName(firstIds[i]) == firstName){
              firstIdsAns.push_back(firstIds[i]);
          }
      } 
  }
  
  //same for second street
  for (int i = 1; i < secondIds.size(); i++){
      if (getStreetName(secondIds[i]) != getStreetName(secondIds[i - 1])){
          secondSame = false;
          break;
      }
  }
  if (secondSame){
      for (int i = 0; i < secondIds.size(); i++){
          secondIdsAns.push_back(secondIds[i]);
      }
  }
  else{
      for (int i = 0; i < secondIds.size(); i++){
          if (getStreetName(secondIds[i]) == secondName){
              secondIdsAns.push_back(secondIds[i]);
              //secondProblem = false;
          }
      } 
  }
  
  //notify users if their inputed text is not specific to a single street
  if (firstIdsAns.size() < 1){
      std::string errorMes = "Error: the string \"" + firstName + "\" does not uniquely identify a single street!\n\nAre you looking for:\n";
      for (int i = 0; i < firstIds.size(); i++){
          errorMes = errorMes + getStreetName(firstIds[i]) + "\n\n";
      }
      //ezgl_app->update_info(errorMes);
      return errorMes;
  } 
  if (secondIdsAns.size() < 1){
      std::string errorMes = "Error: the string \"" + secondName + "\" does not uniquely identify a single street!\n\nAre you looking for:\n";
      for (int i = 0; i < secondIds.size(); i++){
          errorMes = errorMes + getStreetName(secondIds[i]) + "\n\n";
      }
      //ezgl_app->update_info(errorMes);
      return errorMes;
  }
 
  std::vector<int> intersections; //= find_intersections_of_two_streets(std::make_pair(firstIds[0], secondIds[0]));
  //loop through every combination of those streets and add their intersections to the intersection vector 
  for (int i = 0; i < firstIdsAns.size(); i++){
      for (int j = 0; j < secondIdsAns.size(); j++){
          std::pair<int,int> streetPair = std::make_pair(firstIdsAns[i], secondIdsAns[j]);
          std::vector<int> intersects = find_intersections_of_two_streets(streetPair);
          intersections.insert(intersections.end(), intersects.begin(), intersects.end());
      }
  }
  
  //have a special message if the streets do not intersect
  if (intersections.size() == 0){
      //ezgl_app->update_info("These two streets do not intersect!");
      return "Error: " + firstName + " and " + secondName + " do not intersect!";
  }
  
  *ans = intersections;
  return "success!";
}


//drop down menu for switching between maps
gboolean change_map(GtkWidget *, gpointer data)
{
  auto ezgl_app = static_cast<ezgl::application *>(data);
  
  std::string map_path = "/cad2/ece297s/public/maps/";
  //get the unique part of the map path from the id of each selection of the drop down
  map_path = map_path + ezgl_app->get_map_name() + ".streets.bin";
  
  //zoom out all the way before switching maps to avoid map messing up when loading
  std::string main_canvas_id = ezgl_app->get_main_canvas_id();
  auto canvas = ezgl_app->get_canvas(main_canvas_id);
  ezgl::zoom_fit(canvas, canvas->get_camera().get_initial_world());
  
  close_map();
  
  //Load the map and related data structures
  bool load_success = load_map(map_path);
  if(!load_success) {
      std::cerr << "Failed to load map '" << map_path << "'\n";
      return load_success;
  }
  std::cout << "Successfully loaded map '" << map_path << "'\n";
  
  //function to initialize the graphics of the new map once loaded
  reset_map(ezgl_app);

  return TRUE;
}



//toggle walk mode
gboolean toggle_walk(GtkWidget *, gpointer data)
{
    auto ezgl_app = static_cast<ezgl::application *>(data);
    
    ezgl_app->change_visibility();
    
    walk_mode = !walk_mode;
    
    return TRUE;
}
}

