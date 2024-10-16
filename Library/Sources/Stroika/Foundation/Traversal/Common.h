/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Common_h_
#define _Stroika_Foundation_Traversal_Common_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Enumeration.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  Openness is used to define whether an end of a range is open or closed. Open means
     *  not containing the endpoint, and closed means containing the endpoint.
     * 
     *  \note   no support for the concepts of open and closed or neither open nor closed
     *
     *  \note   Configuration::DefaultNames<> supported
     */
    enum class Openness {
        eOpen,
        eClosed,

        Stroika_Define_Enum_Bounds (eOpen, eClosed)
    };
    using Openness::eClosed;
    using Openness::eOpen;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Traversal_Common_h_*/
