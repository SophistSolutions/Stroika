/*
* Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
*/
//  TEST    Foundation::Containers::SortedCollection
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/SortedCollection.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Collection.h"

#include "Stroika/Foundation/Containers/Concrete/SortedCollection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedCollection_stdmultiset.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

using Concrete::SortedCollection_LinkedList;
using Concrete::SortedCollection_stdmultiset;

namespace {
    template <typename CONCRETE_CONTAINER, typename INORDER_COMPARER, typename CONCRETE_CONTAINER_FACTORY>
    void RunTests_ (const INORDER_COMPARER& inorderComparer, CONCRETE_CONTAINER_FACTORY factory)
    {
        typedef typename CONCRETE_CONTAINER::value_type T;
        auto                                            testFunc = [&] (const SortedCollection<T>& s) {
            // verify in sorted order
            optional<T> last;
            for (T i : s) {
                if (last.has_value ()) {
                    EXPECT_TRUE (inorderComparer (*last, i));
                }
                last = i;
            }
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
    void TestOverwriteContainerWhileIteratorRunning_ ()
    {
        SortedCollection<int>    a = {1, 2, 3};
        Traversal::Iterator<int> i = a.begin ();
        // overwrite OK, but
        a = SortedCollection<int>{3, 4, 5};
// cannot access i any longer
#if qDebug && 0
        i++; // assert failure
#endif
    }
}

namespace {
    namespace Test6_SortedCollection_NoDefaultSortFunctionExplicitOne_ {
        using Characters::String;

        struct PrioritizedName {
            String       fName;
            unsigned int fPriority{};

            bool operator== (const PrioritizedName& rhs) const
            {
                if (fName != rhs.fName) {
                    return false;
                }
                if (fPriority != rhs.fPriority) {
                    return false;
                }
                return true;
            }
            String ToString () const
            {
                Characters::StringBuilder sb;
                sb += L"{";
                sb += L"Name: " + Characters::ToString (fName) + L",";
                sb += L"Priority: " + Characters::ToString (fPriority);
                sb += L"}";
                return sb.str ();
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
            PrioritizedNames (const PrioritizedNames& src) = default;

            PrioritizedNames& operator= (PrioritizedNames&& rhs) noexcept = default;
            PrioritizedNames& operator= (const PrioritizedNames& rhs)     = default;

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
#if qHasFeature_GoogleTest
    GTEST_TEST (Foundation_Caching, all)
#else
    void DoRegressionTests_ ()
#endif
    {
        struct MySimpleClassWithoutComparisonOperators_LESS_ : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eStrictInOrder> {
            bool operator() (const SimpleClassWithoutComparisonOperators& lhs, const SimpleClassWithoutComparisonOperators& rhs) const
            {
                return lhs.GetValue () < rhs.GetValue ();
            }
        };

        RunTests_<SortedCollection<size_t>> ();
        RunTests_<SortedCollection<SimpleClass>> ();
        RunTests_<SortedCollection<SimpleClassWithoutComparisonOperators>> (MySimpleClassWithoutComparisonOperators_LESS_{}, [] () {
            return SortedCollection<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_LESS_{});
        });

        RunTests_<SortedCollection_LinkedList<size_t>> ();
        RunTests_<SortedCollection_LinkedList<SimpleClass>> ();
        RunTests_<SortedCollection_LinkedList<SimpleClassWithoutComparisonOperators>> (MySimpleClassWithoutComparisonOperators_LESS_{}, [] () {
            return SortedCollection_LinkedList<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_LESS_{});
        });

        RunTests_<SortedCollection_stdmultiset<size_t>> ();
        RunTests_<SortedCollection_stdmultiset<SimpleClass>> ();
        RunTests_<SortedCollection_stdmultiset<SimpleClassWithoutComparisonOperators>> (MySimpleClassWithoutComparisonOperators_LESS_{}, [] () {
            return SortedCollection_stdmultiset<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_LESS_{});
        });

        TestOverwriteContainerWhileIteratorRunning_ ();
        Test6_SortedCollection_NoDefaultSortFunctionExplicitOne_::DoIt ();

        EXPECT_TRUE (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature" << endl;
#endif
}
