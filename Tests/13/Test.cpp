/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
//  TEST    Foundation::Containers::DataHyperRectangle
//      STATUS  PRELIMINARY
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Concrete/DenseDataHyperRectangle_Vector.h"
#include "Stroika/Foundation/Containers/Concrete/SparseDataHyperRectangle_stdmap.h"
#include "Stroika/Foundation/Containers/DataHyperRectangle.h"
#include "Stroika/Foundation/Containers/DenseDataHyperRectangle.h"
#include "Stroika/Foundation/Containers/SparseDataHyperRectangle.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

namespace {
    namespace Test1_BasicSparseHyperCubeTest_ {
        void RunTests ()
        {
            {
                SparseDataHyperRectangleN<int, 2> x;
                VerifyTestResult (x.empty ());
                VerifyTestResult (x.GetAt (2, 2) == 0);
                x.SetAt (2, 2, 4);
                VerifyTestResult (x.GetAt (2, 2) == 4);
            }
            {
                DataHyperRectangleN<int, 2> x = Concrete::SparseDataHyperRectangle_stdmap<int, size_t, size_t>{};
                Verify (x.GetAt (2, 2) == 0);
                for (auto t : x) {
                }
                x.SetAt (2, 2, 4);
                for (auto t : x) {
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
                DataHyperRectangleN<int, 2> x = Concrete::DenseDataHyperRectangle_Vector<int, size_t, size_t>{3, 4};
                Verify (x.GetAt (2, 2) == 0);
                for (auto t : x) {
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
            VerifyTestResult (tmp.empty ());
            VerifyTestResult (tmp.GetAt (2, 2) == 0);
            tmp.SetAt (2, 2, 4);
            VerifyTestResult (tmp.GetAt (2, 2) == 4);
            Verify (tmp.GetLength () == 1);
            for (auto t : tmp) {
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
            for (auto t : x) {
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Test1_BasicSparseHyperCubeTest_::RunTests ();
        Test2_BasicDenseHyperCubeTest_::RunTests ();
        Test3_BasicSparseHCTest_::RunTests<SparseDataHyperRectangleN<int, 2>> ();
        Test3_BasicSparseHCTest_::RunTests<Concrete::SparseDataHyperRectangle_stdmap<int, size_t, size_t>> ();
        Test4_BasicDenseHCTest_::RunTests<Concrete::DenseDataHyperRectangle_Vector<int, size_t, size_t>> ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
