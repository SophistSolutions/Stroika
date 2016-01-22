/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Range.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;



namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {
#if     qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy
            template    <>
            const   EnumNames<Traversal::Openness>  DefaultNames<Traversal::Openness>::k = EnumNames<Traversal::Openness>::BasicArrayInitializer {
                {
                    { Traversal::Openness::eOpen, L"Open" },
                    { Traversal::Openness::eClosed, L"Closed" },
                }
            };
#else
            template    <>
            const   EnumNames<Traversal::Openness>  DefaultNames<Traversal::Openness>::k {
                EnumNames<Traversal::Openness>::BasicArrayInitializer {
                    {
                        { Traversal::Openness::eOpen, L"Open" },
                        { Traversal::Openness::eClosed, L"Closed" },
                    }
                }
            };
#endif
        }
    }
}
