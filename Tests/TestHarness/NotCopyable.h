/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_TestHarness_NotCopyable_h_
#define _Stroika_TestHarness_NotCopyable_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika::TestHarness {

    struct NotCopyable {
        NotCopyable ()                   = default;
        NotCopyable (const NotCopyable&) = delete;
        NotCopyable (NotCopyable&&)      = default;

        nonvirtual NotCopyable& operator= (const NotCopyable&) = delete;
        nonvirtual NotCopyable& operator= (NotCopyable&&) = default;

        void method (){};
        void const_method () const {};
    };
}

#endif /* _Stroika_TestHarness_NotCopyable_h_ */
