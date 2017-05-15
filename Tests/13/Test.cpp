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
    void DoRegressionTests_ ()
    {
        {
            SparseDataHyperRectangle2<int> x;
            VerifyTestResult (x.empty ());
            VerifyTestResult (x.GetAt (2, 2) == 0);
            x.SetAt (2, 2, 4);
            VerifyTestResult (x.GetAt (2, 2) == 4);
        }
        {
            DataHyperRectangle2<int> x = Concrete::DenseDataHyperRectangle_Vector<int, size_t, size_t>{3, 4};
            Verify (x.GetAt (2, 2) == 0);
            for (auto t : x) {
                int breakhere = 1;
                int b2        = 3;
            }
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

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
