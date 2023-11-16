/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
