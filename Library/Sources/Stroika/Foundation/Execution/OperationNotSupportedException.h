/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_OperationNotSupportedException_h_
#define _Stroika_Foundation_Execution_OperationNotSupportedException_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"

#include "Exceptions.h"

namespace Stroika::Foundation::Execution {

    using Characters::String;

    /**
     *  \see also FeatureNotSupportedException
     */
    class OperationNotSupportedException : public Execution::RuntimeErrorException<> {
    public:
        OperationNotSupportedException ();
        OperationNotSupportedException (const String& operationName);

    public:
        nonvirtual optional<String> GetOperationName () const;

    private:
        optional<String> fOperationName_;
    };

}

#endif /*_Stroika_Foundation_Execution_OperationNotSupportedException_h_*/
