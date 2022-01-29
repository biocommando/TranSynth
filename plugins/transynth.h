#ifndef TRANSYNTH_H
#define TRANSYNTH_H

#include "pluginutil.h"
#include <time.h>

char all_params[][10] = {
    "Tri", "Saw", "Sqr",
    "Wt", "WtW", "Dtn", "Cut", "Res",
    "Dist", "LFO_f", "ModPt", "ModFl",
    "Vol", "Noise", ""};

Parameter *get_param_at_stage(const char *name, int stage)
{
    char buf[64];
    sprintf(buf, "%s@%d", name, stage);
    return get_parameter_ptr(buf);
}

double *get_param_at_stage_v(const char *name, int stage)
{
    Parameter *p = get_param_at_stage(name, stage);
    if (p == NULL)
        return &parameters[LAST_PARAM].value;
    return &p->value;
}

#define STAGE_P(name, stage) (*get_param_at_stage_v(name, stage))

int stage_p_idx = 0;
#define FOR_EACH_STAGE_P_NAME(name) \
    stage_p_idx = 0;                \
    for (char *name = all_params[0]; all_params[stage_p_idx][0]; name = all_params[++stage_p_idx])

#define INC_EXC_PARAMETERS(name, mode, ...)                  \
    {                                                        \
        char param_names_to_check[][10] = {__VA_ARGS__, ""}; \
        int want_skip = mode == 'i';                         \
        for (int i = 0; param_names_to_check[i][0]; i++)     \
        {                                                    \
            if (!strcmp(name, param_names_to_check[i]))      \
            {                                                \
                want_skip = mode == 'i' ? 0 : 1;             \
                break;                                       \
            }                                                \
        }                                                    \
        LOG_TRACE("Skip param %s? %d", name, want_skip);     \
        if (want_skip)                                       \
            continue;                                        \
    }

#define SKIP_PARAMETERS(name, ...) INC_EXC_PARAMETERS(name, 'e', __VA_ARGS__)
#define ONLY_PARAMETERS(name, ...) INC_EXC_PARAMETERS(name, 'i', __VA_ARGS__)

int srand_called = 0;
double random()
{
    if (!srand_called)
    {
        srand((int)time(NULL));
        srand_called = 1;
    }
    return (double)(rand() % 10000) / 10000.0;
}

#define GET_LINK_STATUS_MASK(link_status) \
    ((link_status)[0] | ((link_status)[1] << 1) | ((link_status)[2] << 2) | ((link_status)[3] << 3))

#define FROM_LINK_STATUS_MASK(link_status, mask) \
    do                                           \
    {                                            \
        link_status[0] = (mask)&1 ? 1 : 0;       \
        link_status[1] = (mask)&2 ? 1 : 0;       \
        link_status[2] = (mask)&4 ? 1 : 0;       \
        link_status[3] = (mask)&8 ? 1 : 0;       \
    } while (0)

int link_mode_from_host[4], link_mode_from_host_init = 0;
void read_link_mode()
{
    if (link_mode_from_host_init)
        return;
    for (int stage = 1; stage <= 4; stage++)
    {
        link_mode_from_host[stage - 1] = STAGE_P("link_mode", stage);
    }
    link_mode_from_host_init = 1;
}

void set_linked(const char *name, int stage, double value, int *link_status_mask)
{
    read_link_mode();
    const int *link_mode = link_mode_from_host;

    int link_status[4];
    FROM_LINK_STATUS_MASK(link_status, *link_status_mask);
    LOG_TRACE("set_linked called %s@%d = %lf", name, stage, value);
    LOG_TRACE("link mode = [%d, %d, %d, %d]", link_mode[0], link_mode[1], link_mode[2], link_mode[3]);
    LOG_TRACE("initial link status = [%d, %d, %d, %d]", link_status[0], link_status[1], link_status[2], link_status[3]);

    // Parameter already linked
    if (link_status[stage - 1])
    {
        LOG_TRACE("parameter is already set by previous link operation");
        return;
    }

    STAGE_P(name, stage) = value;

    int current_stage_link_id = link_mode[stage - 1];
    if (current_stage_link_id == 0)
    {
        LOG_TRACE("link id set to '0' which means 'do not link'");
        return;
    }

    LOG_TRACE("setting linked stages to same value");
    link_status[stage - 1] = 1;
    for (int link_stage = 1; link_stage <= 4; link_stage++)
    {
        if (current_stage_link_id == link_mode[link_stage - 1] && !link_status[link_stage - 1])
        {
            LOG_TRACE("link conditions met for %s@%d", name, link_stage);
            link_status[link_stage - 1] = 1;
            STAGE_P(name, link_stage) = value;
        }
    }
    *link_status_mask = GET_LINK_STATUS_MASK(link_status);
    LOG_TRACE("New link status mask is %x", *link_status_mask);
}

#endif