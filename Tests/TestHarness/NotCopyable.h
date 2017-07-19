/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_TestHarness_not_copyable_h_
#define _Stroika_TestHarness_not_copyable_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika {
    namespace TestHarness {

        struct not_copyable {
            not_copyable ()                    = default;
            not_copyable (const not_copyable&) = delete;
            not_copyable (not_copyable&&)      = default;

            void method (){};
            void const_method () const {};
        };
    }
};

#endif /* _Stroika_TestHarness_not_copyable_h_ */
