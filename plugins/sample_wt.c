/*#define LOGGING
#define LOG_LEVEL LOG_LEVEL_TRACE*/
#include "wt_out_streamer.h"

FILE *wt_data_f;

WT_PLUGIN_INIT()
{
    char wave_name[16];
    for (int i = 0; i < 8; i++)
    {
        char buf[] = "namex";
        sprintf(buf, "name%d", i);
        wave_name[i] = (char)PS(buf);
    }
    wave_name[8] = 0;
    strcat(wave_name, ".bin");
    FILE_PATH(wave_name, wave_path);
    wt_data_f = fopen(wave_path, "rb");
}

void destroy_wt_plugin()
{
    fclose(wt_data_f);
}

void calculate_wavetable(float *data, int size)
{
    fread(data, sizeof(float), size, wt_data_f);
}