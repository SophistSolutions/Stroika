/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_TestHarness_NotCopyable_h_
#define _Stroika_TestHarness_NotCopyable_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika {
    namespace TestHarness {

        struct NotCopyable {
            NotCopyable ()                   = default;
            NotCopyable (const NotCopyable&) = delete;
            NotCopyable (NotCopyable&&)      = default;

            const NotCopyable& operator= (const NotCopyable&) = delete;

            void method (){};
            void const_method () const {};
        };
    }
};

#endif /* _Stroika_TestHarness_NotCopyable_h_ */
