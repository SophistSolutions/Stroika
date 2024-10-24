/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
using namespace Stroika::Foundation::Characters::Literals;

using Characters::CompareOptions;

namespace {
    void SimplestCollectionTest_ ()
    {
        /*
         *  A Collection is the simplest form of Stroika container. You can add things, and remove them, and iterate over them.
         *  The order items appear (in iteration) is undefined.
         *  There are almost no requirements on the type of things you can add to a collection (except that it must be copyable). In particular,
         *  there is no need for comparison operators etc to be defined.
         *  View the class declaration, with all the methods well documented, must with examples of usage.
         */
        Collection<int> c;
        c.Add (3);
        c += 13;
        for ([[maybe_unused]] int i : c) {
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

            // 'c' will now operate identically (same API) - but use a different backend datastructure for storage,
            // always showing O(1) worst case addition time
            c = Concrete::Collection_stdforward_list<int>{c};
            c += 4;
        }
        {
            using Characters::String;
            Collection<String> fruits;
            fruits += "apple";
            fruits += "bananas";
            fruits += "cherries";
            fruits += "APPLE";
            // Print (to debugger/tracelog) the fruits - but now they could come out in any order
            DbgTrace ("fruits={}"_f, fruits);
            Assert (fruits.size () == 4); // they are all there

            // Like changing the backend. But this still respects all the rules of a Collection (no order specified) -
            // except now it will happen to be ordered (using the default compare function)
            fruits = SortedCollection<String>{fruits};
            DbgTrace ("sorted fruits={}"_f, fruits);
            Assert (fruits.size () == 4); // only one apple or the other (case squished)
            // note they must now be in alphabetic order
            Assert (fruits.SequentialEquals (initializer_list<String>{"APPLE", "apple", "bananas", "cherries"}));

            // But, we can do the same thing with a compare function that sorts case insensitively
            fruits = SortedCollection<String>{String::LessComparer{CompareOptions::eCaseInsensitive}, fruits};
            DbgTrace ("sorted case insensitive fruits={}"_f, fruits);
            Assert (fruits.SequentialEquals (initializer_list<String>{"apple", "APPLE", "bananas", "cherries"}) or
                    fruits.SequentialEquals (initializer_list<String>{"APPLE", "apple", "bananas", "cherries"}));
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
        Assert (c.SequentialEquals (aVector) or not c.SequentialEquals (aVector));

        vector<int> v2 = c.As<vector<int>> ();
        // V will contain all the same elements as aVector, but maybe not in the same order.
        // it will be in the order the 'c' collection happened to produce
    }
}

namespace {
    void PrintTheContentsOfAContainerToTheTraceLog_ ()
    {
        /*
         *  Use DbgTrace (and often implicitly Characters::ToString ()) to echo objects to a tracelog file (and/or debugger output under windows)
         */
        Debug::TraceContextBumper ctx{L"PrintTheContentsOfAContainerToTheTraceLog_"};
        Collection<int>           tmp{1, 3, 5, 7, 9};
        DbgTrace ("tmp={}"_f, tmp);
    }
}

namespace {
    void UseLinqLikeFunctionalAPIs_ ()
    {
        Debug::TraceContextBumper ctx{L"PrintTheContentsOfAContainerToTheTraceLog_"};
        /*
         *  See the Iterable<> template in Iterable.h - for a ton more of this functional style linq-like operations
         *  you can do on any Stroika container.
         * 
         *  While most of these 'linq like' APIs are also present in STL as '<algorithms>' - those algorithms
         *  are much hard to use (due to how templates work, and how you must pass 2 iterators, and they aren't
         *  member functions of the container).
         */
        {
            Collection<int> tmp{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            auto            whereTestResult = tmp.Where ([] (int i) { return i % 2 == 1; });
            DbgTrace ("whereTestResult={}"_f, whereTestResult);
        }
        {
            Collection<int> tmp{1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
            auto            d = tmp.Distinct ();
            DbgTrace ("d={}"_f, d);
            Assert (d.SetEquals (initializer_list<int>{1, 2, 3, 4, 5}));
        }
        {
            using Characters::String;
            Collection<String> fruits;
            fruits += "apple";
            fruits += "APPLE";
            fruits += "bananas";
            fruits += "cherries";
            DbgTrace ("fruits={}"_f, fruits.Distinct (String::EqualsComparer{CompareOptions::eCaseInsensitive}));
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
    /*
     *  This is just like STL containers. You cannot (generally) update a container while iterating,
     *  except that some APIs allow the iterators to be updated as part of the update process (like erase).
     * 
     *  Unlike STL (generally - some implementations may offer this) - in DEBUG builds, Stroika will detect use
     *  of an invalid iterator and trigger an assertion.
     */
    void UpdatingContainerWhileIterating_ ()
    {
        using Characters::String;
        Collection<String> fruits;
        fruits += "apple";
        fruits += "APPLE";
        fruits += "bananas";
        fruits += "cherries";

        {
            // Note how Stroika iterators have a type that depends on the type of thing you are iterating over, but
            // UNLIKE STL containers, their type does NOT depend on the type of the underlying container you are iterating over.
            // This is a great simplication of APIs (passing iterators to functions no longer requires templates), but
            // comes at a slight cost (operator++ involves a virtual function call to get the next item).
            Iterator<String> i = fruits.Find ([] (String i) { return i == "apple"; });
            Assert (i != fruits.end ());
            Assert (fruits.size () == 4);
        }
        for (Iterator<String> i = fruits.begin (); i != fruits.end ();) {
            if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(*i, "apple")) {
                fruits.Remove (i, &i); // 'i' has already been updated to refer to the next element, regardless of how the Collection<> represents its data
                // If you forget to pass in &i and continue iterating, this is a DETECTED bug (in debug builds)
                // And stroika will assert out when you next use the iterator (i++).
            }
            else {
                ++i;
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
    UseLinqLikeFunctionalAPIs_ ();
    CollectionOfThingsWithNoOpEqualsAndNotDefaultConstructibleEtc_ ();
    UpdatingContainerWhileIterating_ ();
}
