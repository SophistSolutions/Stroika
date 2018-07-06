/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_SilentException_h_
#define _Stroia_Foundation_Execution_SilentException_h_ 1

#include "../StroikaPreComp.h"

#include "Exceptions.h"

namespace Stroika::Foundation {
    namespace Execution {

        /**
         * Throw this when an error has already been reported - so that it isn't reported again
         */
        class SilentException {
        public:
            static const SilentException kThe;
        };
    }
}

#endif /*_Stroia_Foundation_Execution_SilentException_h_*/
