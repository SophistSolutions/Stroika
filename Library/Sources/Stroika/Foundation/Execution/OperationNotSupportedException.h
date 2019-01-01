/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_OperationNotSupportedException_h_
#define _Stroika_Foundation_Execution_OperationNotSupportedException_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"

#include "StringException.h"

namespace Stroika::Foundation::Execution {

    using Characters::String;

    class OperationNotSupportedException : public StringException {
    public:
        OperationNotSupportedException ();
        OperationNotSupportedException (const String& operationName);

    public:
        nonvirtual Characters::String GetOperationName () const;

    private:
        String fOperationName_;
    };

}

#endif /*_Stroika_Foundation_Execution_OperationNotSupportedException_h_*/
