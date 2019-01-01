/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Set.h"
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
    void SimplestSetTest_ ()
    {
        /*
         *  A Set<T> is an un-ordered container where each item of type T is present at most one time.
         */
        {
            Set<int> s;
            s.Add (3);
            s += 3;
            for ([[maybe_unused]] int i : s) {
                Lambda_Arg_Unused_BWA (i); // warning C4189: 'i': local variable is initialized but not referenced
                Assert (i == 3);
            }
            Assert (s.size () == 1);
        }
        {
            vector<int> c{3, 4, 5};
            Set<int>    s9{1, 2, 3};
            Set<int>    s10{Common::DeclareEqualsComparer ([](int l, int r) { return l == r; }), c};
            Assert (s10.Contains (3) and not s10.Contains (88));
        }
    }
}

void Samples::Containers::Set::RunDemo ()
{
    SimplestSetTest_ ();
}