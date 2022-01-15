#include "wt_out_streamer.h"
#include <math.h>

double compr_status = 1;
double simple_limiter(double in)
{
    const double threshold = 0.9;
    const double speed = 0.95;
    double out = in * compr_status;
    if (fabs(out) > threshold)
        compr_status *= speed;
    else if (compr_status > 1)
        compr_status /= speed;
    return out;
}

#define PI 3.141592653589793
typedef struct
{
    double phase;
    double inc;
    double amp;
} Osc;
#define NUM_OSC 32

int osc_first_init = 1;
Osc oscs[NUM_OSC];

double process_osc(Osc *o)
{
    o->phase += o->inc;
    return sin(o->phase) * o->amp;
}

double random() { return ((rand() % 10000) / 10000.0); }

int cnt = 0;
void create_random_oscillators()
{
    const double amp_factor = 0.9;
    double base_amp = 0.5;

    for (int i = 0; i < NUM_OSC; i++)
    {
        oscs[i].inc = default_freq_hz / 44100 * PI * 2 * (i + 1);
        if (osc_first_init)
        {
            oscs[i].phase = random() * PI * 2;
        }
        else if (++cnt % 2)
        {
            oscs[i].inc = default_freq_hz / 44100 * PI * 2 * (NUM_OSC - i);
        }
        oscs[i].amp = 0.5 * (1 + random()) * base_amp * (i % 2 ? -1 : 1);
        base_amp *= amp_factor;
    }
    osc_first_init = 0;
}

WT_PLUGIN_INIT()
{
    unsigned seed = P(seed);
    srand(seed ? seed : 0xf23b65ae);
    create_random_oscillators();
}

int total_idx = 0;

void calculate_wavetable(float *data, int size)
{
    for (int i = 0; i < size; i++)
    {
        data[i] = 0;
        for (int oi = 0; oi < NUM_OSC; oi++)
        {
            if (++total_idx == 882)
            {
                total_idx = 0;
                create_random_oscillators();
            }
            data[i] += process_osc(&oscs[oi]);
        }
        data[i] = simple_limiter(data[i]);
    }
}

void destroy_wt_plugin()
{
}