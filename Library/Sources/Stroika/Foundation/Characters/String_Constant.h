/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Constant_h_
#define _Stroika_Foundation_Characters_String_Constant_h_ 1

#include "../StroikaPreComp.h"

#include "Concrete/String_ExternalMemoryOwnership_ApplicationLifetime.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Characters {

    /**
     *  \brief String_Constant allows creation of String objects with fewer memory allocations, and more efficient storage, but only in constrained situations
     *
     *      String_Constant can safely be used to create Stroika String's from constant C-strings
     *  (string literals) with a minimum of performance cost.
     *
     *  The resulting String is a perfectly compliant Stroika String, and can be modified. It's just
     *  that the initial copy is nearly free.
     *
     *  \em WARNING - BE VERY CAREFUL - this is just an alias for String_ExternalMemoryOwnership_ApplicationLifetime - so be
     *      sure arguments have application lifetime.
     *
     */
    using String_Constant = Concrete::String_ExternalMemoryOwnership_ApplicationLifetime;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "String_Constant.inl"

#endif /*_Stroika_Foundation_Characters_String_Constant_h_*/
