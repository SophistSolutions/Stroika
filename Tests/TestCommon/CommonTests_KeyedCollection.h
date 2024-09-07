/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_KeyedCollection_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_KeyedCollection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Set.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

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
                int operator() (const T1& t) const
                {
                    return t.key;
                };
            };
            using T1_Traits = KeyedCollection_DefaultTraits<T1, int, T1_Key_Extractor>;
#if qHasFeature_GoogleTest
            template <typename CONTAINER, typename TEST_FUNCTION>
            void RunTest (CONTAINER coll, TEST_FUNCTION testFunction)
            {
                EXPECT_TRUE (coll.empty ());
                testFunction (coll);
                coll.Add (T1{1, 101});
                EXPECT_EQ (coll.size (), 1u);
                EXPECT_EQ (coll.Lookup (1)->value, 101);
                coll.Add (T1{1, 201});
                EXPECT_EQ (coll.size (), 1u);
                EXPECT_TRUE (coll.Contains (1));
                EXPECT_EQ (coll.Lookup (1)->value, 201);
                EXPECT_TRUE (not coll.Contains (2));
                auto prevValue = coll;
                EXPECT_EQ (prevValue, coll);
                coll.Add (T1{2, 102});
                EXPECT_TRUE (prevValue != coll);
                EXPECT_TRUE (coll.Contains (2));
                EXPECT_EQ (coll.size (), 2u);
                EXPECT_EQ (coll.Keys (), (Set<int>{1, 2}));
                EXPECT_TRUE (not coll.RemoveIf (99));
                EXPECT_EQ (coll.size (), 2u);
                coll.Remove (1);
                EXPECT_EQ (coll.size (), 1u);
                testFunction (coll);
            };
#endif
        }

#if qHasFeature_GoogleTest
        /**
         */
        template <typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
        void SimpleKeyedCollectionTest_TestBasics (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
        {
            Debug::TraceContextBumper ctx{"CommonTests::CollectionTests::SimpleKeyedCollectionTest_TestBasics"};
            Test1_Basics_::RunTest (factory (), applyToContainer);
        }
#endif

    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_KeyedCollection_h_ */
