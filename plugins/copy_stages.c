#define LOGGING
#define LOG_LEVEL LOG_LEVEL_INFO
#define CONSOLE_LOGGING
#include "transynth.h"

PLUGIN()
{
    int from = P(from);
    int to = P(to);
        
    FOR_EACH_STAGE_P_NAME(name)
    {
        LOG("Copy %s from %d to %d. Original value %lf.", name, from, to, STAGE_P(name, to));
        LOG("To-be-copied value %lf.", STAGE_P(name, from));
        STAGE_P(name, to) = STAGE_P(name, from);
        LOG("New value %lf.", STAGE_P(name, to));
    }
}