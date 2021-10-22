/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Cast_h_
#define _Stroika_Foundation_Debug_Cast_h_ 1

#include "../StroikaPreComp.h"

#include "Assertions.h"

/**
 *  \file
 *
 */

namespace Stroika::Foundation::Debug {

    /**
     *  \brief return the same value as dynamic_cast<T> would have, except instead of checking and throwing
     *         only check in DEBUG builds, and there ASSERT != null.
     *
     *  Use where you would want to check Assert (dynamic_cast<const T*> (&ir) != nullptr) and then do static_cast<> instead;)
     * 
     *      AssertMember (&ir, IteratorRep_);
     *      auto&   mir = static_cast<const IteratorRep_&> (ir);
     * 
     *  The purpose of this function is to gain the performance of static_cast<> but the safety / checking of dynamic_cast (in debug builds).
     * 
     *  \par Example Usage
     *      \code
     *          auto&   mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
     *      \endcode
     * 
     *  \see https://stackoverflow.com/questions/28002/regular-cast-vs-static-cast-vs-dynamic-cast
     */
    template <typename T, typename T1>
    T UncheckedDynamicCast (T1&& arg);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Cast.inl"

#endif /*_Stroika_Foundation_Debug_Cast_h_*/
