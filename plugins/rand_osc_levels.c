/*#define LOGGING
#define CONSOLE_LOGGING
#define LOG_LEVEL LOG_LEVEL_TRACE*/
//#define LOG_FILTER "transynth.c"
#include "transynth.h"

PLUGIN()
{
    FOR_EACH_STAGE_P_NAME(name)
    {
        ONLY_PARAMETERS(name, "Tri", "Saw", "Sqr", "Wt");
        int link_status = 0;
        for (int stage = 1; stage <= 4; stage++)
        {
            set_linked(name, stage, random(), &link_status);
        }
    }
}