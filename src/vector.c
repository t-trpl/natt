#include "types.h"
#include "stdio.h"

temperature_data* vec_init()
{

    temperature_data* ptr = (temperature_data*)malloc(sizeof(temperature_data));
    ptr->temp_points = (temp_point*)malloc(sizeof(temp_point) * 16);
    ptr->temp_size = 0;
    ptr->capacity = 16;
    return ptr;
}

void print_data(temperature_data* vec) 
{
    for (int i = 0; i < vec->capacity; i++) {
        if (i < vec->temp_size) {
            printf("time:%d color:%d marked:%d\n", vec->temp_points[i].time, vec->temp_points[i].color, vec->temp_points[i].marked);
        }
        else {
            printf("time:%d color:%d marked:%d\n", 0, 0, 0);
            vec->temp_points[i].color++;
        }
    }
}

void push_back(temperature_data* vec, temp_point point)
{
    int size = vec->temp_size;
    int capacity = vec->capacity;
    if (size >= capacity) {
        printf("ind:%d cap:%d\n", size, capacity);
        int new_cap = 2 * capacity;
        temp_point* new = (temp_point*)malloc(sizeof(temp_point) * new_cap); 
        temp_point* old = vec->temp_points;
        memcpy(new, old, sizeof(temp_point) * capacity);
        vec->temp_points = new;
        free(old);
        vec->capacity = new_cap;
    }
    vec->temp_points[vec->temp_size++] = point;
}

int next_power_of_two(int size) 
{   
    unsigned n = (unsigned)size;
    if (n == 0) return 1;     
    n--;     
    n |= n >> 1;     
    n |= n >> 2;     
    n |= n >> 4;     
    n |= n >> 8;     
    n |= n >> 16;     
    n++;     
    return (int)n; 
}


void purge_marked(temperature_data* vec) 
{
    int size = vec->temp_size;
    for (int i = 0; i < vec->temp_size; i++) {
        if (vec->temp_points[i].marked) {
            size--;
        }
    }
    int new_cap = next_power_of_two(size);
    temp_point* new = (temp_point*)malloc(sizeof(temp_point) * new_cap);
    int j = 0;
    for (int i = 0; i < vec->temp_size; i++) {
        if (!vec->temp_points[i].marked) {
            new[j] = vec->temp_points[i];
            j++;
        }
    }
    vec->temp_size = j;
    vec->capacity = new_cap;
    free(vec->temp_points);
    vec->temp_points = new;
}
