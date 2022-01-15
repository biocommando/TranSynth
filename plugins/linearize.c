#include "transynth.h"

// Make linear curve from Attack to Release

PLUGIN()
{
    FOR_EACH_STAGE_P_NAME(name)
    {
        double total_diff = STAGE_P(name, 4) - STAGE_P(name, 1);
        double diff_per_stage = total_diff / 3;
        STAGE_P(name, 2) = STAGE_P(name, 1) + diff_per_stage;
        STAGE_P(name, 3) = STAGE_P(name, 2) + diff_per_stage;
    }
}