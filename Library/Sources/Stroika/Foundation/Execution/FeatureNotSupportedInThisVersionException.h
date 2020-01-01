/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_FeatureNotSupportedInThisVersionException_h_
#define _Stroika_Foundation_Execution_FeatureNotSupportedInThisVersionException_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"

#include "Exceptions.h"

namespace Stroika::Foundation::Execution {

    using Characters::String;

    /**
     */
    class FeatureNotSupportedInThisVersionException : public Execution::RuntimeErrorException<> {
    public:
        FeatureNotSupportedInThisVersionException (const wstring& feature);

    public:
        wstring fFeature;
    };

}
#endif /*_Stroika_Foundation_Execution_FeatureNotSupportedInThisVersionException_h_*/
