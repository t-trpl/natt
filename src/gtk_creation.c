#include "main.h"
#include "types.h"
#include "xsctcf.h"
#include <stdio.h>
#include <libayatana-appindicator/app-indicator.h>

void destroy_all_children(GtkWidget* widget)
{
    if (GTK_CONTAINER(widget)) {
        gtk_container_forall(GTK_CONTAINER(widget), (GtkCallback)gtk_widget_destroy, NULL);
    }
}

void on_delete_clicked(GtkWidget* button, gpointer user_data) 
{
    line_data* ld = (line_data*)user_data;
    natt_data* nd = ld->nd;
    GtkWidget* parent = gtk_widget_get_parent(button);
    temp_point* ud = ld->tp;
    ud->marked = true;
    nd->sd->state_change = true;
    gtk_widget_destroy(parent); 
    gtk_widget_show_all(nd->wp->window);
    free(ld);
}

GtkWidget* create_temp_line(natt_data* nd, int index)
{
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, FALSE);

    char buff[20];
    snprintf(buff, sizeof(buff), "%d", nd->td->temp_points[index].color);
    char temp_buff[21];
    snprintf(temp_buff, sizeof(temp_buff), "%s%s", buff, "k");

    int minutes = nd->td->temp_points[index].time % 60;
    int hour = nd->td->temp_points[index].time / 60;

    char hour_buff[10];
    char min_buff[10];
    char out_buff[22];
    snprintf(hour_buff, 10, "%d", hour);
    snprintf(min_buff, 10, "%s%d", minutes < 10 ? "0": "", minutes);
    snprintf(out_buff, sizeof(out_buff), "%s%s%s%s", hour < 10 ? "0": "", hour_buff, ":", min_buff);

    GtkWidget* time = gtk_label_new(out_buff);
    gtk_widget_set_halign(time, GTK_ALIGN_START);
    gtk_widget_set_hexpand(time, TRUE);
    gtk_grid_attach(GTK_GRID(grid), time, 0, 0, 1, 1);

    GtkWidget* temp = gtk_label_new(temp_buff);
    gtk_widget_set_halign(temp, GTK_ALIGN_START);
    gtk_widget_set_hexpand(temp, TRUE);
    gtk_grid_attach(GTK_GRID(grid), temp, 1, 0, 1, 1);

    GtkWidget* button = gtk_button_new_with_label("Delete");
    gtk_widget_set_halign(button, GTK_ALIGN_END);
    gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(button, FALSE);
    gtk_widget_set_vexpand(button, FALSE);

    line_data* ld = (line_data*)malloc(sizeof(line_data));
    ld->nd = nd;
    ld->tp = &nd->td->temp_points[index];

    g_signal_connect(button, "clicked", G_CALLBACK(on_delete_clicked), ld);
    gtk_widget_set_sensitive(GTK_WIDGET(button),!nd->sd->natt_demon_on); 
    gtk_grid_attach(GTK_GRID(grid), button, 2, 0, 1, 1);

    return grid;
}

void update_add_button_sensitivity(GtkWidget* child, gpointer data)
{
    natt_data* nd = (natt_data*)data;
    if (GTK_IS_BUTTON(child) || GTK_IS_ENTRY(child)) {
        gtk_widget_set_sensitive(child, !nd->sd->natt_demon_on);
    }
    else if (GTK_IS_GRID(child)) {
        GtkWidget *button = gtk_grid_get_child_at(GTK_GRID(child), 2, 0);
        if (GTK_IS_BUTTON(button)) {
            gtk_widget_set_sensitive(button, !nd->sd->natt_demon_on);
        }
    }
}

gboolean on_window_delete(GtkWidget* widget, GdkEvent* event, gpointer data)
{
    gtk_widget_hide(widget);
    return TRUE; 
}

void on_quit(GtkMenuItem *item, gpointer data)
{
    gtk_main_quit();
}

void on_tray_icon_clicked(GtkMenuItem* item, gpointer data)
{
    natt_data* nd = (natt_data*) data;
    GtkWidget* window = nd->wp->window;
    gtk_window_deiconify(GTK_WINDOW(window));
    gtk_widget_show_all(window);
}

GtkWidget* create_tray_menu(natt_data* nd) 
{
    GtkWidget* menu = gtk_menu_new();
    GtkWidget* restore_item = gtk_menu_item_new_with_label("Show Window");
    g_signal_connect(restore_item, "activate", G_CALLBACK(on_tray_icon_clicked), nd);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), restore_item);
    GtkWidget* quit_item = gtk_menu_item_new_with_label("Exit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(on_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit_item);
    gtk_widget_show_all(menu);
    return menu;
}

void show_about_dialog(GtkWidget* widget, gpointer data)
{
    GtkWidget* about_dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "natt");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), "0.1 (alpha)");
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(about_dialog), GTK_LICENSE_GPL_3_0);
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), "2025 Tyler Triplett");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), "A simple foss f.lux clone.");
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog), "https://ttrpl.org");
    gtk_about_dialog_set_wrap_license(GTK_ABOUT_DIALOG(about_dialog), TRUE);
    gtk_dialog_run(GTK_DIALOG(about_dialog));
    gtk_widget_destroy(about_dialog);
}

void on_set_button_clicked(GtkWidget* button, gpointer user_data)
{
    set_data* sd = (set_data*)user_data;
    natt_data* nd = sd->nd;
    GtkWidget* entry = sd->entry;
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    int screen_first = 0;
    int screen_last = nd->dd->screens - 1;
    int crtc_specified = -1;
    struct temp_status state;
    state.brightness = 1.0;
    int temp = atoi(text);
    if (!temp) {
        return;
    }
    temp = temp < TEMPERATURE_ZERO ? TEMPERATURE_ZERO : temp;
    state.temp = temp;
    char buff[20];
    snprintf(buff, sizeof(buff), "%d%c", temp, 'k');
    gtk_label_set_text(GTK_LABEL(nd->wp->live_temp), buff);
    for (int screen = screen_first; screen <= screen_last; screen++) {
        sct_for_screen(nd->dd->dpy, screen, crtc_specified, state);
    }
}

void on_new_window_activate(GtkWidget* widget, gpointer user_data)
{
    natt_data* nd = (natt_data*)user_data;
    GtkWidget* parent = nd->wp->window; 
    GtkWidget* new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Set Screen Temp");
    gtk_window_set_resizable(GTK_WINDOW(new_window), FALSE);

    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    GtkWidget* entry_label = gtk_label_new("Temp");
    gtk_grid_attach(GTK_GRID(grid), entry_label, 0, 0, 1, 1);

    GtkWidget* entry = gtk_entry_new();
    int screen_first = 0;
    int crtc_specified = -1;
    struct temp_status start_temp = get_sct_for_screen(nd->dd->dpy, screen_first, crtc_specified); 
    char buff[20];
    snprintf(buff, sizeof(buff), "%d%c", start_temp.temp, 'k');
    gtk_entry_set_text(GTK_ENTRY(entry), buff);
    GtkWidget* button = gtk_button_new_with_label("Set");

    set_data* sd = (set_data*)malloc(sizeof(set_data));
    sd->entry = entry;
    sd->nd = nd;

    g_signal_connect(button, "clicked", G_CALLBACK(on_set_button_clicked), sd);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(new_window), grid);

    gtk_window_set_transient_for(GTK_WINDOW(new_window), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(new_window), TRUE);

    g_signal_connect(new_window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    gtk_widget_show_all(new_window);
}

GtkWidget* create_menu_bar(natt_data* nd) 
{
    GtkWidget* menu_bar = gtk_menu_bar_new();
    GtkWidget* file_menu_item = gtk_menu_item_new_with_label("File");
    GtkWidget* file_menu = gtk_menu_new();

    GtkWidget* menu_item_about = gtk_menu_item_new_with_label("About");
    g_signal_connect(menu_item_about, "activate", G_CALLBACK(show_about_dialog), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item_about);

    GtkWidget* exit_item = gtk_menu_item_new_with_label("Exit");
    g_signal_connect(exit_item, "activate", G_CALLBACK(on_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), exit_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);

    GtkWidget* tools_menu_item = gtk_menu_item_new_with_label("Tools");
    GtkWidget* tools_menu = gtk_menu_new();

    GtkWidget* set_color = gtk_menu_item_new_with_label("Set Color");
    g_signal_connect(set_color, "activate", G_CALLBACK(on_new_window_activate), nd);
    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), set_color);
    gtk_widget_set_sensitive(GTK_WIDGET(set_color),!nd->sd->natt_demon_on); 

    nd->wp->set_color = set_color;

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(tools_menu_item), tools_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), tools_menu_item);
    return menu_bar;
}

void on_switch_toggled(GtkSwitch* sw, gboolean state, gpointer user_data)
{
    natt_data* nd = (natt_data*) user_data;
    state_data* sd = nd->sd;
    widget_pointers* wp = nd->wp;
    temperature_data* td = nd->td;
    
    if (sd->state_change) {
        pthread_mutex_lock(nd->sd->temp_lock);
        temp_point* points = (temp_point*)malloc(sizeof(temp_point) * 100);
        int j = 0;
        for (int i = 0; i < td->temp_size; i++) {
            if (!td->temp_points[i].marked) {
                points[j] = td->temp_points[i];
                j++;
            }
        }
        free(td->temp_points);
        td->temp_points = points;
        td->temp_size = j;
        qsort(td->temp_points, td->temp_size, sizeof(temp_point), compare);
        destroy_all_children(wp->temp_box);
        for (int i = 0; i < td->temp_size; i++) {
            GtkWidget* curr = create_temp_line(nd, i);
            gtk_box_pack_start(GTK_BOX(wp->temp_box), curr, FALSE, FALSE, 0);
        }
        gtk_widget_show_all(wp->temp_box);
        sd->state_change = false;
        pthread_mutex_unlock(nd->sd->temp_lock);
    }
    bool tmp = !atomic_load(&sd->natt_demon_on);
    atomic_store(&sd->natt_demon_on, tmp);
    gtk_switch_set_state(sw, tmp);
    
    gtk_container_forall(GTK_CONTAINER(wp->temp_box), update_add_button_sensitivity, nd);
    gtk_container_forall(GTK_CONTAINER(wp->add_box), update_add_button_sensitivity, nd);
    gtk_widget_set_sensitive(GTK_WIDGET(wp->set_color),!nd->sd->natt_demon_on); 

    write_config(nd);
}

GtkWidget* create_top_bar_widget(natt_data* nd)
{
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    // create current temp box
    GtkWidget* temp_line = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget* temp_label = gtk_label_new("Current Temp: ");

    struct temp_status start_temp = get_sct_for_screen(nd->dd->dpy, 0, -1); 
    char buff[20];
    snprintf(buff, sizeof(buff), "%d%c", start_temp.temp, 'k');
    GtkWidget* temp_value = gtk_label_new(buff);
    nd->wp->live_temp = temp_value;

    gtk_widget_set_halign(temp_line, GTK_ALIGN_START);
    gtk_widget_set_hexpand(temp_line, TRUE);
    gtk_box_pack_start(GTK_BOX(temp_line), temp_label, true, true, 0);
    gtk_box_pack_start(GTK_BOX(temp_line), temp_value, true, true, 0);
    gtk_box_pack_start(GTK_BOX(box), temp_line, true, true, 0);

    // create demon switch
    GtkWidget* switch_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget* switch_label = gtk_label_new("Enable NATT");
    GtkWidget* switch_widget = gtk_switch_new();
    gtk_switch_set_state(GTK_SWITCH(switch_widget), nd->sd->natt_demon_on);
    g_signal_connect(switch_widget, "state-set", G_CALLBACK(on_switch_toggled), nd);
    gtk_box_pack_start(GTK_BOX(switch_box), switch_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(switch_box), switch_widget, FALSE, FALSE, 0);
    gtk_widget_set_halign(switch_box, GTK_ALIGN_END);
    gtk_box_pack_start(GTK_BOX(box), switch_box, true, true, 0);
    return box;
}

GtkWidget* create_temp_box(natt_data* nd)
{
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);

    for (int i = 0; i < nd->td->temp_size; i++) {
        GtkWidget* curr = create_temp_line(nd, i);
        gtk_box_pack_start(GTK_BOX(box), curr, FALSE, FALSE, 0);
    }
    return box;
}

void on_add_click(GtkWidget* button, gpointer user_data)
{
    append_data* ad = (append_data*)user_data;
    natt_data* nd = ad->nd;
    GtkWidget** widgets = ad->widgets;
    const char* time_text = gtk_entry_get_text(GTK_ENTRY(widgets[0])); 
    const char* temp_text = gtk_entry_get_text(GTK_ENTRY(widgets[1])); 
    bool err = false;
    int temp = atoi(temp_text);
    if (!temp) {
        fprintf(stderr, "Invalid temp format: %s\n", temp_text);
        err = true;
    }
    int hours, minutes;
    if (sscanf(time_text, "%d:%d", &hours, &minutes) != 2) {
        fprintf(stderr, "Invalid time format: %s\n", time_text);
        err = true;
    } 
    if (hours < 0 || hours >= 24) {
        //printf("Hour%d", hours);
        fprintf(stderr, "Invalid Hour: %d\n", hours);
        err = true;
    }
    if (minutes < 0 || minutes >= 60) {
        fprintf(stderr, "Invalid Minutes: %d\n", minutes);
        err = true;
    }
    if (err) {
        return;
    }
    nd->sd->state_change = true;
    nd->td->temp_points[nd->td->temp_size].color = temp;
    nd->td->temp_points[nd->td->temp_size].time = 60 * hours + minutes;
    nd->td->temp_points[nd->td->temp_size].marked = false;
    GtkWidget* curr = create_temp_line(nd, nd->td->temp_size);    
    gtk_box_pack_start(GTK_BOX(nd->wp->temp_box), curr, FALSE, FALSE, 0);
    gtk_widget_show_all(nd->wp->window);
    nd->td->temp_size++;
}

GtkWidget* create_append_box(natt_data* nd)
{
    GtkWidget* grid = gtk_grid_new();

    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    GtkWidget* time = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(time), "{24hr}:{min}");
    gtk_grid_attach(GTK_GRID(grid), time, 0, 0, 1, 1);

    gtk_widget_set_sensitive(GTK_WIDGET(time),!nd->sd->natt_demon_on); 

    GtkWidget* temp = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(temp), "{TEMP}k");
    gtk_grid_attach(GTK_GRID(grid), temp, 1, 0, 1, 1);
    gtk_widget_set_sensitive(GTK_WIDGET(temp),!nd->sd->natt_demon_on); 

    append_data* ad = (append_data*) malloc(sizeof(append_data));
    ad->widgets[0] = time;
    ad->widgets[1] = temp;
    ad->nd = nd;

    GtkWidget* button = gtk_button_new_with_label("Add");
    g_signal_connect(button, "clicked", G_CALLBACK(on_add_click), ad);
    gtk_widget_set_sensitive(GTK_WIDGET(button),!nd->sd->natt_demon_on); 
    gtk_grid_attach(GTK_GRID(grid), button, 2, 0, 1, 1);
    return grid;
}

gboolean update_current_label(gpointer user_data)
{
    update_label_data* data = (update_label_data*)user_data;
    int temp = data->temp;
    natt_data* nd = data->nd;
    char buff[20];
    snprintf(buff, sizeof(buff), "%d%c", temp, 'k');
    //printf("%s\n", buff);
    gtk_label_set_text(GTK_LABEL(nd->wp->live_temp), buff);
    return FALSE;
}

void build_app(natt_data* nd)
{
    read_config(nd);

    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "natt");
    gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    nd->wp->window = window;

    // Create the main box
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

     // Create the menu bar
    GtkWidget* menu_bar = create_menu_bar(nd);
    gtk_box_pack_start(GTK_BOX(box), menu_bar, FALSE, FALSE, 0);


    // Create a new box for everything else (content_box)
    GtkWidget* content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);


    // Create the top bar and add it to the content_box
    GtkWidget* top_bar = create_top_bar_widget(nd);
    gtk_box_pack_start(GTK_BOX(content_box), top_bar, FALSE, FALSE, 0);


    // Create the temp widget and add it to the content_box
    nd->wp->temp_box = create_temp_box(nd);
    gtk_box_pack_start(GTK_BOX(content_box), nd->wp->temp_box, FALSE, FALSE, 0);

    // Create the append box and add it to the content_box
    nd->wp->add_box = create_append_box(nd);
    gtk_box_pack_start(GTK_BOX(content_box), nd->wp->add_box, FALSE, FALSE, 0);

    gtk_widget_set_margin_start(content_box, 5);
    gtk_widget_set_margin_end(content_box, 5);
    gtk_widget_set_margin_top(content_box, 5);
    gtk_widget_set_margin_bottom(content_box, 5);

    // Add the content_box to the main box
    gtk_box_pack_start(GTK_BOX(box), content_box, TRUE, TRUE, 0);

    // Add the main box to the window
    gtk_container_add(GTK_CONTAINER(window), box);

    // create tray
    GtkWidget* tray = create_tray_menu(nd);
    AppIndicator* indicator = app_indicator_new("natt-tray", "face-smile", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_menu(indicator, GTK_MENU(tray));

    g_signal_connect(window, "delete-event", G_CALLBACK(on_window_delete), NULL);

    gtk_widget_show_all(window);

    on_clock_daemon(nd);
    g_timeout_add(10000, on_clock_daemon, nd);
}
