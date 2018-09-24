/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/Trace.h"

// Not generally included, but you can include these if you want to select a particular backend implementation
#include "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_stdmap.h"

#include "Stroika/Foundation/Debug/Assertions.h"

#include "Mapping.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

namespace {
    void SimplestMappingTest_ ()
    {
        /*
         *  Mapping which allows for the association of two elements: a key and
         *  a value. The key UNIQUELY specifies its associated value.
         */
        Mapping<int, int> m;
        m.Add (3, -3);
        m.Add (19, -19);
        for (KeyValuePair<int, int> p : m) {
            Assert (p.fKey == -p.fValue);
        }
    }
}

void Samples::Containers::Mapping::RunDemo ()
{
    SimplestMappingTest_ ();
}