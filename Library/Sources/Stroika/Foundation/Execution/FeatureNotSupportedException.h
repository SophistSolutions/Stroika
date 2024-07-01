/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_FeatureNotSupportedException_h_
#define _Stroika_Foundation_Execution_FeatureNotSupportedException_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"

#include "Exceptions.h"

namespace Stroika::Foundation::Execution {

    using Characters::String;

    /**
     *  \see also OperationNotSupportedException
     */
    class FeatureNotSupportedException : public Execution::RuntimeErrorException<> {
    public:
        FeatureNotSupportedException ();
        FeatureNotSupportedException (const String& feature);

    public:
        nonvirtual optional<String> GetFeature () const;

    private:
        optional<String> fFeature_;
    };

}
#endif /*_Stroika_Foundation_Execution_FeatureNotSupportedException_h_*/
