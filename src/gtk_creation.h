#ifndef GTK_CREATION_H
#define GTK_CREATION_H

#include "types.h"

void on_delete_clicked(GtkWidget* button, gpointer user_data);
GtkWidget* create_temp_line(natt_data* nd, int index);
void update_add_button_sensitivity(GtkWidget* child, gpointer data);
gboolean on_window_delete(GtkWidget* widget, GdkEvent* event, gpointer data);
void on_quit(GtkMenuItem *item, gpointer data);
void on_tray_icon_clicked(GtkMenuItem* item, gpointer data);
GtkWidget* create_tray_menu(natt_data* nd);
void show_about_dialog(GtkWidget* widget, gpointer data);
void on_set_button_clicked(GtkWidget* button, gpointer user_data);
void on_new_window_activate(GtkWidget* widget, gpointer user_data);
GtkWidget* create_menu_bar(natt_data* nd);
void on_switch_toggled(GtkSwitch* sw, gboolean state, gpointer user_data);
GtkWidget* create_top_bar_widget(natt_data* nd);
GtkWidget* create_temp_box(natt_data* nd);
void on_add_click(GtkWidget* button, gpointer user_data);
GtkWidget* create_append_box(natt_data* nd);
gboolean update_current_label(gpointer user_data);
void build_app(natt_data* nd);

#endif // GTK_CREATION_H
