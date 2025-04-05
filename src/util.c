#include "main.h"

natt_data* create_natt_info()
{
    natt_data* nd = (natt_data*)malloc(sizeof(natt_data));
    nd->wp = (widget_pointers*)malloc(sizeof(widget_pointers));
    nd->dd = (display_data*)malloc(sizeof(display_data));
    nd->td = vec_init();
    nd->sd = (state_data*)malloc(sizeof(state_data));
    nd->sd->active = false;
    nd->sd->state_change = false;
    nd->sd->natt_demon_on = false;
    nd->sd->temp_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(nd->sd->temp_lock, NULL);
    printf("ok\n");
    return nd;
}

int compare(const void* a, const void* b)
{
    int num1 = ((temp_point*)a)->time;
    int num2 = ((temp_point*)b)->time;
    if (num1 < num2) return -1;
    if (num1 > num2) return 1;
    return 0;
}

char* trim_spaces(char* str)
{
    if (str == NULL || *str == '\0') {
        return str;
    }
    char* start = str;
    while (*start && *start == ' ') {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    char* end = str + strlen(str) - 1;
    while (end >= str && *end == ' ') {
        *end = '\0';
        end--;
    }
    return str;
}

