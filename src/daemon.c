#include "main.h"
#include <time.h>

void* flux(void* arg)
{
    natt_data* nd = (natt_data*) arg;
    int screen_first = 0;
    int screen_last = nd->dd->screens - 1;
    int crtc_specified = -1;
    time_t current_time = time(NULL);
    struct tm* time = localtime(&current_time);
    int current_minute = time->tm_hour * 60 + time->tm_min;
    struct temp_status start_temp = get_sct_for_screen(nd->dd->dpy, screen_first, crtc_specified); 
    int c = nd->td->temp_size - 1;
    pthread_mutex_lock(nd->sd->temp_lock);
    while (c >= 0 && nd->td->temp_points[c].time > current_minute) {
        c--;
    }
    if (c < 0) {
        c = nd->td->temp_size - 1;
    }
    int target_temp = nd->td->temp_points[c].color;
    pthread_mutex_unlock(nd->sd->temp_lock);
    const int step_sleep = 50000;
    struct temp_status state;
    state.temp = start_temp.temp;
    int step = state.temp < target_temp ? 1 : -1;
    state.brightness = 1.0;
    while (atomic_load(&nd->sd->natt_demon_on) && state.temp != target_temp) {
        update_label_data* label_data = (update_label_data*)malloc(sizeof(update_label_data));
        label_data->nd = nd;
        state.temp += step;
        for (int screen = screen_first; screen <= screen_last; screen++) {
            sct_for_screen(nd->dd->dpy, screen, crtc_specified, state);
        }
        label_data->temp = state.temp;
        g_idle_add(update_current_label, label_data);
        usleep(step_sleep);
    }
    atomic_store(&nd->sd->active, false);
    return NULL;
}

gboolean on_clock_daemon(gpointer user_data)
{
    natt_data* nd = (natt_data*)user_data;
    state_data* state = nd->sd;
    temperature_data* temp = nd->td;
    if (!atomic_load(&state->natt_demon_on) || atomic_load(&state->active) || temp->temp_size == 0) {
        return true;
    }
    atomic_store(&state->active, true);
    pthread_t thread;
    pthread_create(&thread, NULL, flux, nd);
    pthread_detach(thread);
    return true;
}
