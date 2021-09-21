/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Containers::KeyedCollection
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/KeyedCollection.h"
#if 0
#include "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_stdmap.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#endif
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_Mapping.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

namespace {
    namespace Test1_ {
        struct T1 {
            int key;
            int value;
        };
        struct T1_Key_Extractor {
            int operator() (const T1& t) const { return t.key; };
        };
        using T1_Traits = KeyedCollection_DefaultTraits<T1, int, equal_to<int>, T1_Key_Extractor>;
        void DoIt ()
        {
#if 1
            auto test = [] (auto coll) {
                VerifyTestResult (coll.empty ());
                coll.Add (T1{1, 101});
                VerifyTestResult (coll.GetLength () == 1);
                coll.Add (T1{1, 201});
                VerifyTestResult (coll.GetLength () == 1);
                coll.Add (T1{2, 102});
                VerifyTestResult (coll.GetLength () == 2);
            };
            KeyedCollection<T1, int> l1{[] (T1 e) { return e.key; }};
            test (l1);
            KeyedCollection<T1, int, T1_Traits> l2{[] (T1 e) { return e.key; }};
            test (l2);
            KeyedCollection<T1, int, T1_Traits> l3;
            test (l3);
#endif

#if 0
            using AA1 = function<bool (ArgByValueType<int>, ArgByValueType<int>)>;
            using AA2 = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, AA1>;
           
            AA1 keyComparer2y11{equal_to<int>{}};
            AA1 keyComparer2y11x = equal_to<int>{};
            AA2 keyComparer2y1{equal_to<int>{}};
            AA2 keyComparer2y = AA2 (equal_to<int>{});

            keyComparer2y.x (equal_to<int>{});
            keyComparer2y.operator=(equal_to<int>{});


            keyComparer2y       = equal_to<int>{};
      AA2 keyComparer2yxx = equal_to<int>{};

      
      function < bool (ArgByValueType<int>, ArgByValueType<int>)> keyComparer111                = equal_to<int>{};
            KeyedCollection<T1, int>::KeyEqualityComparerType                                   keyComparer1{equal_to<int>{}};



            function<bool (ArgByValueType<int>, ArgByValueType<int>)> e             = equal_to<int>{};

            KeyedCollection<T1, int, T1_Traits>::KeyEqualityComparerType keyComparer2xx = function<bool (ArgByValueType<int>, ArgByValueType<int>)>{equal_to<int>{}};
   
            
            
            
            KeyedCollection<T1, int, T1_Traits>::KeyEqualityComparerType keyComparer2{equal_to<int>{}};
       //     KeyedCollection<T1, int, T1_Traits>::KeyEqualityComparerType keyComparer2x = equal_to<int>{};
            KeyedCollection<T1, int, T1_Traits>::KeyEqualityComparerType keyComparer3{KeyedCollection<T1, int, T1_Traits>::TraitsType::DefaultKeyEqualsComparer{}};

#endif
        }
    }

    void DoRegressionTests_ ()
    {
        Test1_::DoIt ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
