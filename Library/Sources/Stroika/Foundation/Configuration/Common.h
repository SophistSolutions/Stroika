/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Common_h_
#define _Stroika_Foundation_Configuration_Common_h_ 1

#include "../Common/Common.h"

namespace Stroika::Foundation::Configuration {

    /**
     *  The assume attribute was introduced in c++23, and Stroika OPTIONALLY supports this, but doesn't require it as of Stroika v3.
     *  So use _ASSUME_ATTRIBUTE_ () to conditionaly use [[assume(X)]]
     */
#if __has_cpp_attribute(assume)
#define _ASSUME_ATTRIBUTE_(X) [[assume (X)]];
#elif _MSC_VER
    // Docs not clear.
    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1774r4.pdf suggests this hack. BUT...
    // https://github.com/MicrosoftDocs/cpp-docs/blob/main/docs/build/optimization-best-practices.md seems to hint __assume doesn't evaluate X, except to pay attention to simple a>constant compares so this should be OK
#define _ASSUME_ATTRIBUTE_(X) __assume (X);
#else
#define _ASSUME_ATTRIBUTE_(X)
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Configuration_Common_h_*/
