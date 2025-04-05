#ifndef VECTOR_H
#define VECTOR_H

#include "types.h"

temperature_data* vec_init();
void print_data(temperature_data* vec);
void push_back(temperature_data* vec, temp_point point);
int next_power_of_two(int size);
void purge_marked(temperature_data* vec);

#endif // VECTOR_H
