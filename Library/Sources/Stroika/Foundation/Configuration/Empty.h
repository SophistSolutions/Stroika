/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Empty_h_
#define _Stroika_Foundation_Configuration_Empty_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Configuration {

    /**
     *  Believe it or not, this is often useful in template metaprogramming. Because you cannot declare a data member as 'void'. This amounts to same thing if used with [[no_unique_address]]
     */
    struct Empty {};

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Configuration_Empty_h_*/
