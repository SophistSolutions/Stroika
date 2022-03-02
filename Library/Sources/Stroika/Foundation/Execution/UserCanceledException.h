/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_UserCanceledException_h_
#define _Stroia_Foundation_Execution_UserCanceledException_h_ 1

#include "../StroikaPreComp.h"

#include "SilentException.h"

namespace Stroika::Foundation::Execution {

    /**
     *  mostly treat the same as SilentException
     */
    class UserCanceledException : public SilentException {
    public:
        static const UserCanceledException kThe;
    };
    inline const UserCanceledException UserCanceledException::kThe;

}

#endif /*_Stroia_Foundation_Execution_UserCanceledException_h_*/
