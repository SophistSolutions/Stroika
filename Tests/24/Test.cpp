/*
* Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
*/
//  TEST    Foundation::Containers::SortedCollection
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/SortedCollection.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Collection.h"

#include "Stroika/Foundation/Containers/Concrete/SortedCollection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedCollection_SkipList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedCollection_stdmultiset.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::AsIntsThreeWayComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::SortedCollection_LinkedList;
using Concrete::SortedCollection_SkipList;
using Concrete::SortedCollection_stdmultiset;

#if qStroika_HasComponent_googletest
namespace {
    template <typename CONCRETE_CONTAINER, typename INORDER_COMPARER, typename CONCRETE_CONTAINER_FACTORY>
    void RunTests_ (const INORDER_COMPARER& inorderComparer, CONCRETE_CONTAINER_FACTORY factory)
    {
        typedef typename CONCRETE_CONTAINER::value_type T;
        auto                                            testFunc = [&] (const SortedCollection<T>& s) {
            // verify in sorted order
            EXPECT_TRUE (s.IsOrderedBy (inorderComparer));
        };
        CommonTests::CollectionTests::SimpleCollectionTest_Generic<CONCRETE_CONTAINER> (factory, testFunc);
    }
    template <typename CONCRETE_CONTAINER>
    void RunTests_ ()
    {
        RunTests_<CONCRETE_CONTAINER> (std::less<typename CONCRETE_CONTAINER::value_type>{}, [] () { return CONCRETE_CONTAINER{}; });
    }
}

namespace {
    namespace Test6_SortedCollection_NoDefaultSortFunctionExplicitOne_ {
        using Characters::String;

        struct PrioritizedName {
            String       fName;
            unsigned int fPriority{};

            bool   operator== (const PrioritizedName& rhs) const = default;
            String ToString () const
            {
                Characters::StringBuilder sb;
                sb << "{"sv;
                sb << "Name: "sv << fName << ","sv;
                sb << "Priority: "sv << fPriority;
                sb << "}"sv;
                return sb;
            }
        };

        constexpr auto kDefaultPrioritizedName_OrderByDefault_Less =
            Stroika::Foundation::Common::DeclareInOrderComparer ([] (const PrioritizedName& lhs, const PrioritizedName& rhs) -> bool {
                if (lhs.fPriority > rhs.fPriority) {
                    return true;
                }
                else if (lhs.fPriority < rhs.fPriority) {
                    return false;
                }
                return lhs.fName < rhs.fName;
            });

        struct PrioritizedNames : Containers::SortedCollection<PrioritizedName> {
            PrioritizedNames ()
                : SortedCollection<PrioritizedName>{kDefaultPrioritizedName_OrderByDefault_Less}
            {
            }
            PrioritizedNames (const PrioritizedNames&) = default;

            PrioritizedNames& operator= (PrioritizedNames&&) noexcept = default;
            PrioritizedNames& operator= (const PrioritizedNames&)     = default;

            String GetName () const
            {
                for (auto i : *this) {
                    return i.fName;
                }
                return L"Unknown"sv;
            }
            void AddName (const String& name, unsigned int priority)
            {
                for (auto i = begin (); i != end (); ++i) {
                    if (i->fName == name) {
                        if (priority > i->fPriority) {
                            Update (i, PrioritizedName{name, priority});
                        }
                        return;
                    }
                }
                Add (PrioritizedName{name, priority});
            }
        };

        void DoIt ()
        {
            PrioritizedNames      t1, t2;
            [[maybe_unused]] bool t = (t1 == t2);
        }

    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedCollection, DEFAULT_FACTORY)
    {
        Debug::TraceContextBumper ctx{"{}::DEFAULT_FACTORY"};
        RunTests_<SortedCollection<size_t>> ();
        RunTests_<SortedCollection<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<SortedCollection<OnlyCopyableMoveable>> (AsIntsLessComparer<OnlyCopyableMoveable>{}, [] () {
            return SortedCollection<OnlyCopyableMoveable>{AsIntsLessComparer<OnlyCopyableMoveable>{}};
        });
        RunTests_<SortedCollection<OnlyCopyableMoveable>> (AsIntsLessComparer<OnlyCopyableMoveable>{}, [] () {
            return SortedCollection<OnlyCopyableMoveable>{AsIntsThreeWayComparer<OnlyCopyableMoveable>{}};
        });
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedCollection, SortedCollection_LinkedList)
    {
        Debug::TraceContextBumper ctx{"{}::SortedCollection_LinkedList"};
        RunTests_<SortedCollection_LinkedList<size_t>> ();
        RunTests_<SortedCollection_LinkedList<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<SortedCollection_LinkedList<OnlyCopyableMoveable>> (AsIntsLessComparer<OnlyCopyableMoveable>{}, [] () {
            return SortedCollection_LinkedList<OnlyCopyableMoveable> (AsIntsLessComparer<OnlyCopyableMoveable>{});
        });
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedCollection, SortedCollection_SkipList)
    {
        Debug::TraceContextBumper ctx{"{}::SortedCollection_SkipList"};
        RunTests_<SortedCollection_SkipList<size_t>> ();
        RunTests_<SortedCollection_SkipList<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<SortedCollection_SkipList<OnlyCopyableMoveable>> (AsIntsLessComparer<OnlyCopyableMoveable>{}, [] () {
            return SortedCollection_SkipList<OnlyCopyableMoveable> (AsIntsThreeWayComparer<OnlyCopyableMoveable>{});
        });
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedCollection, SortedCollection_stdmultiset)
    {
        Debug::TraceContextBumper ctx{"{}::SortedCollection_stdmultiset"};
        RunTests_<SortedCollection_stdmultiset<size_t>> ();
        RunTests_<SortedCollection_stdmultiset<OnlyCopyableMoveableAndTotallyOrdered>> ();
        RunTests_<SortedCollection_stdmultiset<OnlyCopyableMoveable>> (AsIntsLessComparer<OnlyCopyableMoveable>{}, [] () {
            return SortedCollection_stdmultiset<OnlyCopyableMoveable> (AsIntsLessComparer<OnlyCopyableMoveable>{});
        });
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedCollection, TestOverwriteContainerWhileIteratorRunning_)
    {
        Debug::TraceContextBumper ctx{"{}::TestOverwriteContainerWhileIteratorRunning_"};
        SortedCollection<int>     a = {1, 2, 3};
        Traversal::Iterator<int>  i = a.begin ();
        // overwrite OK, but
        a = SortedCollection<int>{3, 4, 5};
// cannot access i any longer
#if qStroika_Foundation_Debug_AssertionsChecked && 0
        i++; // assert failure
#endif
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedCollection, Test6_SortedCollection_NoDefaultSortFunctionExplicitOne_)
    {
        Debug::TraceContextBumper ctx{"{}::Test6_SortedCollection_NoDefaultSortFunctionExplicitOne_"};
        Test6_SortedCollection_NoDefaultSortFunctionExplicitOne_::DoIt ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_SortedCollection, CLEANUPS)
    {
        EXPECT_TRUE (OnlyCopyableMoveableAndTotallyOrdered::GetTotalLiveCount () == 0 and OnlyCopyableMoveable::GetTotalLiveCount () == 0); // simple portable leak check
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
