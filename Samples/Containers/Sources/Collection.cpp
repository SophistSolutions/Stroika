/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/SortedCollection.h"
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

using Characters::CompareOptions;

namespace {
    void SimplestCollectionTest_ ()
    {
        /*
         *  A Collection is the simplest form of Stroika container. You can add things, and remove them, and iterate over them.
         *  View the class declaration, with all the methods well documented, must with examples of usage.
         */
        Collection<int> c;
        c.Add (3);
        c += 13;
        for ([[maybe_unused]] int i : c) {
            Lambda_Arg_Unused_BWA (i); // warning C4189: 'i': local variable is initialized but not referenced
            Assert (i == 3 or i == 13);
        }
        Assert (c.size () == 2);
    }
}

namespace {
    void UseParticularConcreteRepresentation_ ()
    {
        {
            Collection<int> c{3, 5, 19, 3040, 34, 1, 33, 33, 4, 19};

            // Unclear what the performance characteristics of this will be - with a linked list - O(1), but with array, O(N) worst case.
            c += 4;

            // 'c' will now operate identically (same API) - but use a differnt backend datastructure for storage,
            // always showing O(1) worst case addition time
            c = Concrete::Collection_stdforward_list<int>{c};
            c += 4;
        }
        {
            using Characters::String;
            Collection<String> fruits;
            fruits += L"apple";
            fruits += L"bananas";
            fruits += L"cherries";
            fruits += L"APPLE";
            // Print (to debugger/tracelog) the fruits - but now they could come out in any order
            DbgTrace (L"fruits=%s", Characters::ToString (fruits).c_str ());
            Assert (fruits.size () == 4); // they are all there

            // Like changing the backend. But this still respects all the rules of a Collection (no order specified) -
            // except now it will happen to be ordered (using the default compare function)
            fruits = SortedCollection<String>{fruits};
            DbgTrace (L"sorted fruits=%s", Characters::ToString (fruits).c_str ());
            Assert (fruits.size () == 4); // only one apple or the other (case squished)
            // note they must now be in alphabetic order
            Assert (fruits.SequnceEquals (initializer_list<String>{L"APPLE", L"apple", L"bananas", L"cherries"}));

            // But, we can do the same thing with a compare function that sorts case insensitively
            fruits = SortedCollection<String>{String::LessComparer{CompareOptions::eCaseInsensitive}, fruits};
            DbgTrace (L"sorted case insensitve fruits=%s", Characters::ToString (fruits).c_str ());
            Assert (fruits.SequnceEquals (initializer_list<String>{L"apple", L"APPLE", L"bananas", L"cherries"}) or
                    fruits.SequnceEquals (initializer_list<String>{L"APPLE", L"apple", L"bananas", L"cherries"}));
        }
    }
}

namespace {
    void InterfaceWithSTLContainers_ ()
    {
        vector<int>     aVector{1, 3, 5, 7, 9, 11};
        Collection<int> c{aVector};

        // CANNOT guarantee the ordering is the same, as Collection guarantees it keeps all the same elements,
        // but does not guarantee maintaining order.
        Assert (c.SetEquals (aVector));
        Assert (c.SequnceEquals (aVector) or not c.SequnceEquals (aVector));

        vector<int> v2 = c.As<vector<int>> ();
        // V will contain all the same elements as aVector, but maybe not in the same order.
        // it will be in the order the 'c' collection happened to produce
    }
}

namespace {
    void PrintTheContentsOfAContainerToTheTraceLog_ ()
    {
        /*
         *  Use DbgTrace and Characters::ToString () to echo objects to a tracelog file (and/or debugger output under windows)
         */
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
        /*
         *  See the Iterable<> template in Iterable.h - for a ton more of this functional style linq-like operations
         *  you can do on any Stroika container.
         */
        {
            Collection<int> tmp{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            auto            whereTestResult = tmp.Where ([] (int i) { return i % 2 == 1; });
            DbgTrace (L"tmp=%s", Characters::ToString (whereTestResult).c_str ());
        }
        {
            Collection<int> tmp{1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
            auto            d = tmp.Distinct ();
            DbgTrace (L"d=%s", Characters::ToString (d).c_str ());
            Assert (d.SetEquals (initializer_list<int>{1, 2, 3, 4, 5}));
        }
        {
            using Characters::String;
            Collection<String> fruits;
            fruits += L"apple";
            fruits += L"APPLE";
            fruits += L"bananas";
            fruits += L"cherries";
            DbgTrace (L"fruits=%s", Characters::ToString (fruits.Distinct (String::EqualsComparer{CompareOptions::eCaseInsensitive})).c_str ());
            Assert (fruits.Distinct (String::EqualsComparer{CompareOptions::eCaseInsensitive}).size () == 3); // only one apple or the other (case squished)
        }
    }
}

namespace {
    void CollectionOfThingsWithNoOpEqualsAndNotDefaultConstructibleEtc_ ()
    {
        struct MyFunnyObject_ {
            MyFunnyObject_ () = delete; // no need to be default constructible
            MyFunnyObject_ (int /*n*/)
            {
                // need some constructor to test
            }
            MyFunnyObject_ (const MyFunnyObject_&) = default; // OK, but you do need to be copyable
        };
        Collection<MyFunnyObject_> myObjects;
        myObjects.Add (MyFunnyObject_{3});
        myObjects += MyFunnyObject_{4}; // use whatever add syntax you want

        // THIS WONT WORK, because no operator== defined
        //      myObjects.Remove (MyFunnyObject_{3});
        // but this will remove the first object found
        Assert (myObjects.size () == 2);
        myObjects.Remove (MyFunnyObject_{3}, [] (const MyFunnyObject_&, const MyFunnyObject_&) { return true; });
        Assert (myObjects.size () == 1);
    }
}

namespace {
    void IteratorsAndSafeUpdateIteration_ ()
    {
        using Characters::String;
        Collection<String> fruits;
        fruits += L"apple";
        fruits += L"APPLE";
        fruits += L"bananas";
        fruits += L"cherries";

        {
            Iterator<String> i = fruits.FindFirstThat ([] (String i) { return i == L"apple"; });
            Assert (i != fruits.end ());
            Assert (fruits.size () == 4);
        }
        for (Iterator<String> i = fruits.begin (); i != fruits.end (); ++i) {
            if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(*i, L"apple")) {
                fruits.Remove (i);
                // with STL containers, it would be illegal to reference i again, as in i++.
                // However, with Stroika iterators, they are smart about doing the right thing,
                // when they point to a deleted item, and this code will work as expected.
            }
        }
        Assert (fruits.size () == 2);
    }
}

void Samples::Containers::Collection::RunDemo ()
{
    SimplestCollectionTest_ ();
    UseParticularConcreteRepresentation_ ();
    InterfaceWithSTLContainers_ ();
    PrintTheContentsOfAContainerToTheTraceLog_ ();
    StoreOtherSortsOfElements_ ();
    UseLinqLikeFunctionalAPIs_ ();
    CollectionOfThingsWithNoOpEqualsAndNotDefaultConstructibleEtc_ ();
    IteratorsAndSafeUpdateIteration_ ();
}
