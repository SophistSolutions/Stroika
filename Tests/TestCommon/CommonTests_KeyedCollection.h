/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_KeyedCollection_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_KeyedCollection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Set.h"

#include "../TestHarness/TestHarness.h"

/**
 */

namespace CommonTests {
    namespace KeyedCollectionTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

        namespace Test1_Basics_ {
            struct T1 {
                int key;
                int value;
            };
            struct T1_Key_Extractor {
                int operator() (const T1& t) const { return t.key; };
            };
            using T1_Traits = KeyedCollection_DefaultTraits<T1, int, T1_Key_Extractor>;
            template <typename CONTAINER, typename TEST_FUNCTION>
            void RunTest (CONTAINER coll, TEST_FUNCTION testFunction)
            {
                VerifyTestResult (coll.empty ());
                testFunction (coll);
                coll.Add (T1{1, 101});
                VerifyTestResult (coll.size () == 1);
                VerifyTestResult (coll.Lookup (1)->value == 101);
                coll.Add (T1{1, 201});
                VerifyTestResult (coll.size () == 1);
                VerifyTestResult (coll.Contains (1));
                VerifyTestResult (coll.Lookup (1)->value == 201);
                VerifyTestResult (not coll.Contains (2));
                auto prevValue = coll;
                VerifyTestResult (prevValue == coll);
                coll.Add (T1{2, 102});
                VerifyTestResult (prevValue != coll);
                VerifyTestResult (coll.Contains (2));
                VerifyTestResult (coll.size () == 2);
                VerifyTestResult ((coll.Keys () == Set<int>{1, 2}));
                VerifyTestResult (not coll.RemoveIf (99));
                VerifyTestResult (coll.size () == 2);
                coll.Remove (1);
                VerifyTestResult (coll.size () == 1);
                testFunction (coll);
            };
        }

        /**
         */
        template <typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
        void SimpleKeyedCollectionTest_TestBasics (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
        {
            Debug::TraceContextBumper ctx{L"CommonTests::CollectionTests::SimpleKeyedCollectionTest_TestBasics"};
            Test1_Basics_::RunTest (factory (), applyToContainer);
        }

    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_KeyedCollection_h_ */
