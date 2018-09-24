#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/Trace.h"

// Not generally included, but you can include these if you want to select a particular backend implementation
#include "Stroika/Foundation/Containers/Concrete/Set_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Set_stdset.h"

#include "Stroika/Foundation/Debug/Assertions.h"

#include "Set.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

namespace {
    void SimplestSetTest_()
    {
        /*
         *  A Set<T> is an un-ordered container where each item of type T is present at most one time.
         */
        Set<int> s;
        s.Add (3);
        s += 3;
        for (int i : s) {
            Assert (i == 3);
        }
        Assert (s.size () == 1);
    }
}

void Samples::Containers::Set::RunDemo ()
{
    SimplestSetTest_ ();
}