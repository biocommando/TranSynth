/*#define LOGGING
#define LOG_LEVEL LOG_LEVEL_TRACE*/
#include "wt_out_streamer.h"

FILE *wt_data_f;

void read_wave_name(char *wave_name)
{
    FOR_EACH_PARAMETER(i)
    {
        if (parameters[i].name[0] == '$')
        {
            const char *_wave_name = &parameters[i].name[1];
            strcpy(wave_name, _wave_name);
            return;
        }
    }
}

WT_PLUGIN_INIT()
{
    char wave_name[64];
    for (int i = 0; i < 8; i++)
    {
        char buf[] = "namex";
        sprintf(buf, "name%d", i);
        wave_name[i] = (char)PS(buf);
    }
    wave_name[8] = 0;
    if (!wave_name[0])
        read_wave_name(wave_name);
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