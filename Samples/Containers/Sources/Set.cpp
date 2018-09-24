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

#include "Set.h"

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
        Set<int> s;
        s.Add (3);
        for (int i : s) {
            Assert (i == 3);
        }
    }
}

void Samples::Containers::Set::RunDemo ()
{
    SimplestMappingTest_ ();
}