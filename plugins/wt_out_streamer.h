#ifndef WT_OUT_STREAMER_H
#define WT_OUT_STREAMER_H

#include "pluginutil.h"

#define default_freq_hz 50.0

void calculate_wavetable(float *data, int size);
void destroy_wt_plugin();

char dir_path[1024];

#define FILE_PATH(file, out) \
    char out[1024];          \
    strcpy(out, dir_path);   \
    strcat(out, file)

void get_dir_path(const char *s)
{
    strcpy(dir_path, s);
    for (int i = strlen(dir_path) - 1; dir_path[i] != '\\' && dir_path[i] != '/'; i--)
        dir_path[i] = 0;
}

void save_wavetable_data()
{
    FILE_PATH("wavedata.bin", data_fname);
    float buf[100];
    FILE *f = fopen(data_fname, "wb");
    for (int i = 0; i < 10000; i += 100)
    {
        calculate_wavetable(buf, 100);
        fwrite(buf, sizeof(float), 100, f);
    }
    fclose(f);

    new_parameter("wt_generated", 1);
}

#define WT_PLUGIN_INIT()       \
    void wt_plugin_init();     \
    int main(MAIN_ARGS)        \
    {                          \
        INIT;                  \
        get_dir_path(argv[1]); \
        wt_plugin_init();      \
        save_wavetable_data(); \
        FINALIZE;              \
    }                          \
    void wt_plugin_init()

#endif