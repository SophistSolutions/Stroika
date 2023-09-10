/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Containers::KeyedCollection
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>
#include <typeindex>

#include "Stroika/Foundation/Containers/Concrete/KeyedCollection_Array.h"
#include "Stroika/Foundation/Containers/Concrete/KeyedCollection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/KeyedCollection_stdhashset.h"
#include "Stroika/Foundation/Containers/Concrete/KeyedCollection_stdset.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_KeyedCollection.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

namespace {
    namespace Test_KeyedCollectionTypeIndexUsesStdSet_ {
        namespace Private_ {
            struct Obj_ {
                type_index fTypeIndex;  // KEY
                int        otherData{}; //
            };
            struct My_Extractor_ {
                auto operator() (const Obj_& t) const -> type_index
                {
                    return t.fTypeIndex;
                };
            };
            using My_Traits_ = Containers::KeyedCollection_DefaultTraits<Obj_, type_index, My_Extractor_>;
        }
        void RunAll ()
        {
            using namespace Private_;
            {
                Concrete::KeyedCollection_stdset<Obj_, type_index, My_Traits_> s1;
                KeyedCollection<Obj_, type_index, My_Traits_>                  s2;
                s2.Add (Obj_{typeid (int)});
                s2.Add (Obj_{typeid (long int)});
            }
            {
                // Or slighltly more flexiblely, but less efficiently (cuz extractor stored in std::function and invoked through that wrapper)
                KeyedCollection<Obj_, type_index> s2{My_Extractor_{}};
                s2.Add (Obj_{typeid (int)});
                s2.Add (Obj_{typeid (long int)});
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        {
            using T1               = CommonTests::KeyedCollectionTests::Test1_Basics_::T1;
            using T1_Traits        = CommonTests::KeyedCollectionTests::Test1_Basics_::T1_Traits;
            using T1_Key_Extractor = CommonTests::KeyedCollectionTests::Test1_Basics_::T1_Key_Extractor;
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return KeyedCollection<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return KeyedCollection<T1, int, T1_Traits>{T1_Key_Extractor{}}; }, [] (auto) {});
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics ([] () { return KeyedCollection<T1, int, T1_Traits>{}; },
                                                                                     [] (auto) {});
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return Concrete::KeyedCollection_Array<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return Concrete::KeyedCollection_LinkedList<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return Concrete::KeyedCollection_stdset<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return Concrete::KeyedCollection_stdhashset<T1, int>{[] (T1 e) { return e.key; }}; }, [] (auto) {});
        }
        Test_KeyedCollectionTypeIndexUsesStdSet_::RunAll ();

        VerifyTestResult (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
