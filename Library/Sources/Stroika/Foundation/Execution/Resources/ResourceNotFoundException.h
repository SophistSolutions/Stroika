/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_ResourceNotFoundException_h_
#define _Stroika_Foundation_Execution_Resources_ResourceNotFoundException_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"

#include "../StringException.h"

namespace Stroika::Foundation::Execution {

    using Characters::String;

    class ResourceNotFoundException : public StringException {
    public:
        ResourceNotFoundException ();

    public:
        /**
         */
        static const ResourceNotFoundException kThe;
    };
}

#endif /*_Stroika_Foundation_Execution_Resources_ResourceNotFoundException_h_*/
