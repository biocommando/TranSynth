/*#define LOGGING
#define LOG_LEVEL LOG_LEVEL_TRACE*/
#include "wt_out_streamer.h"

FILE *wt_data_f = NULL;
const int FMT_FLOAT32 = 0;
const int FMT_INT8 = 1;
const int FMT_INT16 = 2;

int format;

WT_PLUGIN_INIT()
{
    char wave_name[STR_LENGTH];
    if (get_string_parameter("", wave_name) >= 0)
    {
        char _wave_name[STR_LENGTH];
        snprintf(_wave_name, STR_LENGTH, "%s.bin", wave_name);
        FILE_PATH(_wave_name, wave_path);
        wt_data_f = fopen(_wave_name, "rb");
        format = P(fmt);
    }
}

void destroy_wt_plugin()
{
    if (wt_data_f)
        fclose(wt_data_f);
}

void calculate_wavetable(float *data, int size)
{
    if (!wt_data_f || format < FMT_FLOAT32 || format > FMT_INT16)
    {
        memset(data, 0, sizeof(float) * size);
        return;
    }

    if (format == FMT_FLOAT32)
    {
        fread(data, sizeof(float), size, wt_data_f);
    }
    if (format == FMT_INT8)
    {
        char *buf = (char *)malloc(size);
        fread(buf, 1, size, wt_data_f);
        for (int i = 0; i < size; i++)
        {
            data[i] = buf[i] / (float)(1 << 7);
        }
        free(buf);
    }
    if (format == FMT_INT16)
    {
        short *buf = (short *)malloc(size * sizeof(short));
        fread(buf, sizeof(short), size, wt_data_f);
        for (int i = 0; i < size; i++)
        {
            data[i] = buf[i] / (float)(1 << 15);
        }
        free(buf);
    }
}