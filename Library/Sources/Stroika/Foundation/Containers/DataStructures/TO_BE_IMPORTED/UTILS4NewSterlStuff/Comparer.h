#pragma once

#include <cstring>
#include <string>

#include "Config.h"

/*
 Sorted containers place requirments on the contained elements to allow ordering. The interface used here mimics strcmp, because that
 can be sort items more efficiently in some cases (strings, for example). The default implementation requires definitions for == and <. There
 are a variety of partial specializations to make things more efficient (strings, numbers), and get correct behavior (pointers).
 */

template <typename T>
struct Comparer {
    static int Compare (T v1, T v2)
    {
        if (v1 == v2) {
            return 0;
        }
        return (v1 < v2) ? -1 : 1;
    }
};

/*
 Most of the time if people use pointers for keys, they wanted comparisons on the things pointer at, not the pointers themselves
 */
template <typename T>
struct Comparer<T*> {
    static int Compare (T* v1, T* v2)
    {
        RequireNotNull (v1);
        RequireNotNull (v2);
        return Comparer<T>::Compare (*v1, *v2);
    }
};

// Numerics
template <>
struct Comparer<short> {
    typedef short VType;
    static int    Compare (VType v1, VType v2)
    {
        return (v1 - v2);
    }
};

template <>
struct Comparer<int> {
    typedef int VType;
    static int  Compare (VType v1, VType v2)
    {
        return (v1 - v2);
    }
};

template <>
struct Comparer<long> {
    typedef long VType;
    static VType Compare (VType v1, VType v2)
    {
        return (v1 - v2);
    }
};

// strings
template <>
struct Comparer<char*> {
    static int Compare (char* v1, char* v2)
    {
        return (strcmp (v1, v2));
    }
};

template <>
struct Comparer<std::string> {
    static int Compare (const std::string& v1, const std::string& v2)
    {
        return v1.compare (v2);
    }
};
