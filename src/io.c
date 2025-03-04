#include "main.h"
#include <fcntl.h>
#include <sys/stat.h>

void write_config(natt_data* nd)
{
    const char* home = getenv("HOME");
    const char* config_dir = "/.config";
    const char* config_file = "/.config/natt.conf";
    if (!home) {
        fprintf(stderr, "No home...\n");
        return;
    }
    char dir_path[1024];
    snprintf(dir_path, sizeof(dir_path), "%s%s", home, config_dir);
    struct stat st;
    if (stat(dir_path, &st) == -1) {
        if (mkdir(dir_path, 0775) == -1) {
            fprintf(stderr, "Failed to create config directory...\n");
            return;
        }
    }
    char file_path[1024];
    snprintf(file_path, sizeof(file_path), "%s%s", home, config_file);
    int f = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (f == -1) {
        fprintf(stderr, "Failed to open config...\n");
        return;
    }
    char first_line[4];
    snprintf(first_line, sizeof(first_line), "%d\n", nd->sd->natt_demon_on ? 1 : 0);
    if (write(f, first_line, strlen(first_line)) == -1) {
        fprintf(stderr, "Failed to write config...\n");
        close(f);
        return;
    }
    char contents[4096] = "";
    for (int i = 0; i < nd->td->temp_size; i++) {
        char line[128];
        int hour = nd->td->temp_points[i].time / 60;
        int minutes = nd->td->temp_points[i].time % 60;
        int temp = nd->td->temp_points[i].color;
        snprintf(line, sizeof(line), "%02d:%02d, %d\n", hour, minutes, temp);
        strncat(contents, line, sizeof(contents) - strlen(contents) - 1);
    }
    if (write(f, contents, strlen(contents)) == -1) {
        fprintf(stderr, "Failed to write config...\n");
    }
    close(f);
}

void read_config(natt_data* nd)
{
    const char* home = getenv("HOME");
    const char* dest = "/.config/natt.conf";
    if (!home) {
        fprintf(stderr, "No home...\n");
        return;
    }
    char path[1024];
    snprintf(path, sizeof(path), "%s%s", home, dest);
    FILE *config;
    temperature_data* temp_data = nd->td;
    state_data* sd = nd->sd;
    temp_data->temp_points = (temp_point*)malloc(sizeof(temp_point) * 100);
    temp_point* temp_points = temp_data->temp_points;
    temp_data->temp_size = 0;
    if (access(path, F_OK) != -1) {
        config = fopen(path, "r");
        char line[100];
        while (fgets(line, sizeof(line), config)) {
            if (strlen(line) == 0 || line[0] == '#') {
                continue;
            }
            if (strlen(line) == 2) {
                int enable = atoi(line);
                sd->natt_demon_on = enable;
                continue;
            }
            char* prefix = trim_spaces(strtok(line, ":"));
            char* suffix = trim_spaces(strtok(NULL, ","));
            char* stemp = trim_spaces(strtok(NULL, ","));
            //printf("%s\n%s\n%s\n", prefix, suffix, stemp);
            int time = atoi(prefix) * 60 + atoi(suffix);
            int temp = atoi(stemp);
            int ind = temp_data->temp_size;
            temp_data->temp_points[ind].time = time;
            temp_points[ind].color = temp;
            temp_points[ind].marked = 0;
            temp_data->temp_size++;
        }
        qsort(temp_points, temp_data->temp_size, sizeof(temp_point), compare);
        fclose(config);
    }
}
