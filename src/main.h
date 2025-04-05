#ifndef MAIN_H
#define MAIN_H

#include "types.h"
#include "xsctcf.h"

void build_app(natt_data* nd);
void write_config(natt_data* nd);
char* trim_spaces(char* str);
void read_config(natt_data* nd);
int compare(const void* a, const void *b);
void on_delete_clicked(GtkWidget* button, gpointer user_data);
GtkWidget* create_temp_line(natt_data* nd, int index);
void update_add_button_sensitivity(GtkWidget* child, gpointer data);
void update_add_button_sensitivity(GtkWidget* child, gpointer data);
void on_new_window_activate(GtkWidget* widget, gpointer user_data);
gboolean on_window_delete(GtkWidget* widget, GdkEvent* event, gpointer data);
void on_quit(GtkMenuItem* item, gpointer data);
void on_tray_icon_clicked(GtkMenuItem* item, gpointer data);
GtkWidget* create_tray_menu(natt_data* nd) ;
void show_about_dialog(GtkWidget* widget, gpointer data);
GtkWidget* create_menu_bar(natt_data* nd);
natt_data* create_natt_info();
void on_switch_toggled(GtkSwitch* sw, gboolean state, gpointer user_data);
GtkWidget* create_top_bar_widget(natt_data* nd);
GtkWidget* create_temp_box(natt_data* nd);
void on_add_click(GtkWidget* button, gpointer user_data);
GtkWidget* create_append_box(natt_data* nd);
gboolean update_current_label(gpointer user_data);
void* flux(void* arg);
gboolean on_clock_daemon(gpointer user_data);

// "Vector" Stuff
temperature_data* vec_init();
void push_back(temperature_data* vec, temp_point point);
int next_power_of_two(int size);
void purge_marked(temperature_data* vec); 
void print_vec(temperature_data* vec); 

#endif // MAIN_H
