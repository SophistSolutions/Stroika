/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Immportalize_h_
#define _Stroika_Foundation_Common_Immportalize_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Common {

    /**
     *  Create a singleton of a class initialized once, but whose DTOR is never called.
     *
     *  Based on template <class _Ty> _Ty& _Immortalize() from VS2k19 runtime library....
     */
    template <class T>
    T& Immortalize ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Immortalize.inl"

#endif /*_Stroika_Foundation_Common_Immportalize_h_*/
