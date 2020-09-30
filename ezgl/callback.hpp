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

#ifndef EZGL_CALLBACK_HPP
#define EZGL_CALLBACK_HPP

#include "ezgl/application.hpp"
#include "ezgl/camera.hpp"
#include "ezgl/canvas.hpp"
#include "ezgl/control.hpp"
#include "ezgl/graphics.hpp"

#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "StreetsDatabaseAPI.h"
#include "Globals.h"

#include <iostream>

namespace ezgl {

/**** Callback functions for keyboard and mouse input, and for all the ezgl predefined buttons. *****/

/**
 * React to a <a href = "https://developer.gnome.org/gtk3/stable/GtkWidget.html#GtkWidget-key-press-event">keyboard
 * press event</a>.
 *
 * @param widget The GUI widget where this event came from.
 * @param event The keyboard event.
 * @param data A pointer to any user-specified data you passed in.
 *
 * @return FALSE to allow other handlers to see this event, too. TRUE otherwise.
 */
gboolean press_key(GtkWidget *widget, GdkEventKey *event, gpointer data);

/**
 * React to <a href = "https://developer.gnome.org/gtk3/stable/GtkWidget.html#GtkWidget-button-press-event">mouse click
 * event</a>
 *
 * @param widget The GUI widget where this event came from.
 * @param event The click event.
 * @param data A pointer to any user-specified data you passed in.
 *
 * @return FALSE to allow other handlers to see this event, too. TRUE otherwise.
 */
gboolean press_mouse(GtkWidget *widget, GdkEventButton *event, gpointer data);

/**
 * React to <a href = "https://developer.gnome.org/gtk3/stable/GtkWidget.html#GtkWidget-button-release-event">mouse release
 * event</a>
 *
 * @param widget The GUI widget where this event came from.
 * @param event The click event.
 * @param data A pointer to any user-specified data you passed in.
 *
 * @return FALSE to allow other handlers to see this event, too. TRUE otherwise.
 */
gboolean release_mouse(GtkWidget *widget, GdkEventButton *event, gpointer data);

/**
 * React to <a href = "https://developer.gnome.org/gtk3/stable/GtkWidget.html#GtkWidget-button-release-event">mouse release
 * event</a>
 *
 * @param widget The GUI widget where this event came from.
 * @param event The click event.
 * @param data A pointer to any user-specified data you passed in.
 *
 * @return FALSE to allow other handlers to see this event, too. TRUE otherwise.
 */
gboolean move_mouse(GtkWidget *widget, GdkEventButton *event, gpointer data);

/**
 * React to <a href = "https://developer.gnome.org/gtk3/stable/GtkWidget.html#GtkWidget-scroll-event"> scroll_event
 * event</a>
 *
 * @param widget The GUI widget where this event came from.
 * @param event The click event.
 * @param data A pointer to any user-specified data you passed in.
 *
 * @return FALSE to allow other handlers to see this event, too. TRUE otherwise.
 */
gboolean scroll_mouse(GtkWidget *widget, GdkEvent *event, gpointer data);

/**
 * React to the clicked zoom_fit button
 *
 * @param widget The GUI widget where this event came from.
 * @param data A pointer to any user-specified data you passed in.
 *
 * @return FALSE to allow other handlers to see this event, too. TRUE otherwise.
 */
gboolean press_zoom_fit(GtkWidget *widget, gpointer data);

/**
 * React to the clicked zoom_in button
 *
 * @param widget The GUI widget where this event came from.
 * @param data A pointer to any user-specified data you passed in.
 *
 * @return FALSE to allow other handlers to see this event, too. TRUE otherwise.
 */
gboolean press_zoom_in(GtkWidget *widget, gpointer data);

/**
 * React to the clicked zoom_out button
 *
 * @param widget The GUI widget where this event came from.
 * @param data A pointer to any user-specified data you passed in.
 *
 * @return FALSE to allow other handlers to see this event, too. TRUE otherwise.
 */
gboolean press_zoom_out(GtkWidget *widget, gpointer data);


//find button to do the search for the intersection of two inputted streets in the search bars
gboolean press_find(GtkWidget *, gpointer data);

//press find subfunction to return the intersections of two streets given their names, plus error checking
std::string get_error_message(std::string firstName, std::string secondName, std::vector<int> *ans);

//drop down menu for switching between maps
gboolean change_map(GtkWidget *, gpointer data);

//toggle walk mode
gboolean toggle_walk(GtkWidget *, gpointer data);

}

#endif //EZGL_CALLBACK_HPP
