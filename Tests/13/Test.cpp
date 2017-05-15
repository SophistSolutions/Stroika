/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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
                SparseDataHyperRectangle2<int> x;
                VerifyTestResult (x.empty ());
                VerifyTestResult (x.GetAt (2, 2) == 0);
                x.SetAt (2, 2, 4);
                VerifyTestResult (x.GetAt (2, 2) == 4);
            }
            {
                DataHyperRectangle2<int> x = Concrete::SparseDataHyperRectangle_stdmap<int, size_t, size_t>{};
                Verify (x.GetAt (2, 2) == 0);
                for (auto t : x) {
                    int breakhere = 1;
                    int b2        = 3;
                }
                x.SetAt (2, 2, 4);
                for (auto t : x) {
                    int breakhere = 1;
                    int b2        = 3;
                }
            }
        }
    }
}

namespace {
    namespace Test2_BasicSparseHCTest_ {
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
                int breakhere = 1;
                int b2        = 3;
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Test1_BasicSparseHyperCubeTest_::RunTests ();
        Test2_BasicSparseHCTest_::RunTests<SparseDataHyperRectangle2<int>> ();
        Test2_BasicSparseHCTest_::RunTests<Concrete::SparseDataHyperRectangle_stdmap<int, size_t, size_t>> ();
        {
            DataHyperRectangle2<int> x = Concrete::DenseDataHyperRectangle_Vector<int, size_t, size_t>{3, 4};
            Verify (x.GetAt (2, 2) == 0);
            for (auto t : x) {
                int breakhere = 1;
                int b2        = 3;
            }
        }
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
