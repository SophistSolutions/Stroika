/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_ResourceNotFoundException_h_
#define _Stroika_Foundation_Execution_Resources_ResourceNotFoundException_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

namespace Stroika::Foundation::Execution {

    using Characters::String;

    class ResourceNotFoundException : public Execution::RuntimeErrorException<> {
    public:
        ResourceNotFoundException ();

    public:
        /**
         */
        static const ResourceNotFoundException kThe;
    };
    inline const ResourceNotFoundException ResourceNotFoundException::kThe;

}

#endif /*_Stroika_Foundation_Execution_Resources_ResourceNotFoundException_h_*/
