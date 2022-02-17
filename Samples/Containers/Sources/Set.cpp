/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
         *  Think of it like std::set<T>, except that the Stroika set may be implemented with a variety of
         *  different data structures, and there is no need to define operator< (or less<) comparison function
         *  (except for certain backend data structure representations).
         */
        {
            Set<int> s;
            s.Add (3);
            s += 3;
            for ([[maybe_unused]] int i : s) {
                Assert (i == 3);
            }
            Assert (s.size () == 1);
        }
        {
            vector<int> c{3, 4, 5};
            Set<int>    s9{1, 2, 3};
            // Construct a set with an arbitrary comparison routine (in this case comparing mod 11)
            Set<int> s10{Common::DeclareEqualsComparer ([] (int l, int r) { return (l % 11) == (r % 11); }), c};
            Assert (s10.Contains (3) and s10.Contains (3 + 11) and not s10.Contains (6));
            DbgTrace (L"s10=%s", Characters::ToString (s10).c_str ());
        }
        {
            Set<int> s{1, 2, 3};                  // use the default Set<> representation - the best for type 'int'
            s = Concrete::Set_LinkedList<int>{s}; // Force using a linked list to represent the set
            // other set operations work the same, either way
            if (s.Contains (5)) {
                Assert (false);
            }
        }
    }
}

void Samples::Containers::Set::RunDemo ()
{
    SimplestSetTest_ ();
}