/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_HashBase_h_
#define _Stroika_Foundation_Cryptography_Digest_HashBase_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <concepts>
#include <functional>

#include "Stroika/Foundation/Common/Common.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 * 
 *  \note - this separate file exists to avoid mutual include issues - it logically would make sense to
 *          include this in Hash.h
 */

namespace Stroika::Foundation::Cryptography::Digest {

    /**
     *  \brief check argument FUNCTION is callable with a HASHABLE_T, and produces (something convertible to) size_t
     * 
     * see https://stackoverflow.com/questions/65127936/defining-a-c20-concept-for-hash-functions
     * https://en.cppreference.com/w/cpp/utility/hash
     */
    template <typename FUNCTION, typename HASHABLE_T>
    concept IHashFunction = regular_invocable<FUNCTION, HASHABLE_T> and requires (FUNCTION f, HASHABLE_T t) {
        { invoke (f, t) } -> convertible_to<size_t>;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "HashBase.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_HashBase_h_*/
