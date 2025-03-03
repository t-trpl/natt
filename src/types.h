#ifndef TYPES_H
#define TYPES_H

#include <X11/Xlib.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <X11/extensions/Xrandr.h>
#include <stdatomic.h>

typedef struct widget_pointers {
    GtkWidget* window;
    GtkWidget* temp_box;
    GtkWidget* add_box;
    GtkWidget* live_temp;
    GtkWidget* set_color;
} widget_pointers;

typedef struct temp_point {
    int color;
    int time;
    bool marked;
} temp_point;

typedef struct display_data {
     Display* dpy;
     int screens;

} display_data;

typedef struct temperature_data {
    temp_point* temp_points;
    int temp_size;
} temperature_data;

typedef struct state_data {
    _Atomic bool natt_demon_on;
    bool state_change;
    _Atomic bool active;
    pthread_mutex_t* temp_lock;
} state_data;

typedef struct natt_data {
    widget_pointers* wp;
    display_data* dd;
    temperature_data* td;
    state_data* sd;

} natt_data;

typedef struct line_data {
    natt_data* nd;
    temp_point* tp;
} line_data;

typedef struct append_data {
    natt_data* nd;
    GtkWidget* widgets[2];
} append_data;

typedef struct update_label_data {
    natt_data* nd;
    int temp;
} update_label_data;

typedef struct set_data {
    natt_data* nd;
    GtkWidget* entry;
} set_data;

#endif // TYPES_H
