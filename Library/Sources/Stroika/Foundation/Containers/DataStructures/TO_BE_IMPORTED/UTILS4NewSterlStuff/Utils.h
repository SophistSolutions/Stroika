#pragma once
#include <cstdio>
#include <ctime>
#include <sys/timeb.h>

#include <iostream>
#include <random>

/*
    Utilities used in the test suites and also currently in the container classes (at least RandomSize_t)
*/

#include "../../Shared/Headers/Config.h"

extern size_t        RandomSize_t (size_t first = 1, size_t last = size_t (-1));
extern size_t        NormallyDistributedRandomNumber (size_t first, size_t last);
extern std::mt19937& GetRandomNumberEngine ();

struct Timer {
    Timer ()
    {
        fStartTime = GetCurrentTimeInMilliseconds ();
    }

    ~Timer ()
    {
        double netTicks = GetElapsedTime ();

        char buf[128];
        sprintf (buf, "%lu milliseconds (%5.3f seconds)", (unsigned long)netTicks, netTicks / 1000.0);
        std::cout << buf << std::flush;
    }

    double GetElapsedTime ()
    {
        return GetCurrentTimeInMilliseconds () - fStartTime;
    }

    static double GetCurrentTimeInMilliseconds ()
    {
        timeb t;
        ftime (&t);
        return t.time * 1000.0 + t.millitm;
    }

    double fStartTime;
};

#include "Utils.inl"
