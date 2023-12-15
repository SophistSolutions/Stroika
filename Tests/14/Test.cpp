/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Containers::DataHyperRectangle
//      STATUS  PRELIMINARY
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Concrete/DenseDataHyperRectangle_Vector.h"
#include "Stroika/Foundation/Containers/Concrete/SparseDataHyperRectangle_stdmap.h"
#include "Stroika/Foundation/Containers/DataHyperRectangle.h"
#include "Stroika/Foundation/Containers/DenseDataHyperRectangle.h"
#include "Stroika/Foundation/Containers/SparseDataHyperRectangle.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

#if qHasFeature_GoogleTest
namespace {
    namespace Test1_BasicSparseHyperCubeTest_ {
        void RunTests ()
        {
            {
                SparseDataHyperRectangleN<int, 2> x;
                EXPECT_TRUE (x.empty ());
                EXPECT_TRUE (x.GetAt (2, 2) == 0);
                x.SetAt (2, 2, 4);
                EXPECT_TRUE (x.GetAt (2, 2) == 4);
            }
            {
                SparseDataHyperRectangleN<int, 3> x;
                EXPECT_TRUE (x.empty ());
                EXPECT_TRUE (x.GetAt (1, 2, 3) == 0);
                x.SetAt (1, 2, 3, 99);
                EXPECT_TRUE (x.GetAt (1, 2, 3) == 99);
            }
            {
                DataHyperRectangleN<int, 2> x = Concrete::SparseDataHyperRectangle_stdmapN<int, 2>{};
                Verify (x.GetAt (2, 2) == 0);
                for ([[maybe_unused]] auto t : x) {
                }
                x.SetAt (2, 2, 4);
                for ([[maybe_unused]] auto t : x) {
                }
            }
            {
                DataHyperRectangleN<int, 3> x = Concrete::SparseDataHyperRectangle_stdmapN<int, 3>{};
                Verify (x.GetAt (1, 2, 3) == 0);
                for ([[maybe_unused]] auto t : x) {
                }
                x.SetAt (1, 2, 3, 99);
                for ([[maybe_unused]] auto t : x) {
                }
            }
        }
    }
}

namespace {
    namespace Test2_BasicDenseHyperCubeTest_ {
        void RunTests ()
        {
            {
                DataHyperRectangleN<int, 2> x = Concrete::DenseDataHyperRectangle_VectorN<int, 2>{3, 4};
                Verify (x.GetAt (2, 2) == 0);
                for ([[maybe_unused]] auto t : x) {
                }
            }
        }
    }
}

namespace {
    namespace Test3_BasicSparseHCTest_ {
        template <typename CONCRETE_CONTAINER2>
        void RunTests ()
        {
            CONCRETE_CONTAINER2 tmp{};
            EXPECT_TRUE (tmp.empty ());
            EXPECT_TRUE (tmp.GetAt (2, 2) == 0);
            tmp.SetAt (2, 2, 4);
            EXPECT_TRUE (tmp.GetAt (2, 2) == 4);
            Verify (tmp.size () == 1);
            for ([[maybe_unused]] auto t : tmp) {
            }
        }
    }
}

namespace {
    namespace Test4_BasicDenseHCTest_ {
        template <typename CONCRETE_CONTAINER2>
        void RunTests ()
        {
            DataHyperRectangleN<int, 2> x = CONCRETE_CONTAINER2{3, 4};
            Verify (x.GetAt (2, 2) == 0);
            for ([[maybe_unused]] auto t : x) {
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Test1_BasicSparseHyperCubeTest_::RunTests ();
        Test2_BasicDenseHyperCubeTest_::RunTests ();
        Test3_BasicSparseHCTest_::RunTests<SparseDataHyperRectangleN<int, 2>> ();
        Test3_BasicSparseHCTest_::RunTests<Concrete::SparseDataHyperRectangle_stdmap<int, size_t, size_t>> ();
        Test4_BasicDenseHCTest_::RunTests<Concrete::DenseDataHyperRectangle_Vector<int, size_t, size_t>> ();

        EXPECT_TRUE (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
