#include "../../Shared/Headers/TestingData.h"


const char* DisplayDistribution (DataDistribution d)
{
    switch (d) {
        case eUniformDist:
            return "uniform";
        case eNormalizedDist:
            return "normal";
        case eZipfDist:
            return "zipf";
        default:
            break;
    }
    AssertNotReached ();
    return "";
}
