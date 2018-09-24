#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Debug/Trace.h"

// Not generally included, but you can include these if you want to select a particular backend implementation
#include "Stroika/Foundation/Containers/Concrete/Collection_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_stdforward_list.h"

#include "Stroika/Foundation/Debug/Assertions.h"

#include "Collection.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

namespace {
    void SimplestCollectionTest_ ()
    {
        /*
         *  A Collection is the simplest form of Stroika container. You can add things, and remove them, and iterate over them.
         */
        Collection<int> c;
        c.Add (3);
        c += 13;
        for (int i : c) {
            Assert (i == 3 or i == 13);
        }
        Assert (c.size () == 2);
    }
}

namespace {
    void UseParticularConcreteRepresentation_ ()
    {
        Collection<int> c{3, 5, 19, 3040, 34, 1, 33, 33, 4, 19};

        // Unclear what the performance characteristics of this will be - with a linked list - O(1), but with array, O(N) worst case.
        c += 4;

        // 'c' will not operate identically (same API) - but use a differnt backend datastructure for storage, always showing O(1) worst case addition time
        c = Concrete::Collection_stdforward_list<int>{c};
        c += 4;
    }
}

namespace {
    void InterfaceWithSTLContainers_ ()
    {
        vector<int>     aVector{1, 3, 5, 7, 9, 11};
        Collection<int> c{aVector};

        // CANNOT guarantee the ordering is the same, as Collection guarantees it keeps all the same elements, but does not guarantee maintaining order.
        Assert (c.SetEquals (aVector));
        Assert (c.SequnceEquals (aVector) or not c.SequnceEquals (aVector));

        vector<int> v2 = c.As<vector<int>> ();
        // V will contain all the same elements as aVector, but maybe not in the same order
    }
}

namespace {
    void PrintTheContentsOfAContainerToTheTraceLog_ ()
    {
        Debug::TraceContextBumper ctx{L"PrintTheContentsOfAContainerToTheTraceLog_"};
        Collection<int>           tmp{1, 3, 5, 7, 9};
        DbgTrace (L"tmp=%s", Characters::ToString (tmp).c_str ());
    }
}

namespace {
    void StoreOtherSortsOfElements_ ()
    {
        Debug::TraceContextBumper ctx{L"StoreOtherSortsOfElements_"};
        using Characters::String;
        Collection<String> fruits;
        fruits += L"apple";
        fruits += L"bananas";
        fruits += L"cherries";
        DbgTrace (L"fruits=%s", Characters::ToString (fruits).c_str ());
    }
}
namespace {
    void UseLinqLikeFunctionalAPIs_ ()
    {
        Debug::TraceContextBumper ctx{L"PrintTheContentsOfAContainerToTheTraceLog_"};
        {
            Collection<int> tmp{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            auto            whereTestResult = tmp.Where ([](int i) { return i % 2 == 1; });
            DbgTrace (L"tmp=%s", Characters::ToString (whereTestResult).c_str ());
        }
        {
            Collection<int> tmp{1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
            auto            d = tmp.Distinct ();
            DbgTrace (L"d=%s", Characters::ToString (d).c_str ());
            Assert (d.SetEquals (initializer_list<int> {1, 2, 3, 4, 5}));
        }
    }
}

void Samples::Containers::Collection::RunTests ()
{
    SimplestCollectionTest_ ();
    UseParticularConcreteRepresentation_ ();
    InterfaceWithSTLContainers_ ();
    PrintTheContentsOfAContainerToTheTraceLog_ ();
    StoreOtherSortsOfElements_ ();
    UseLinqLikeFunctionalAPIs_ ();
}