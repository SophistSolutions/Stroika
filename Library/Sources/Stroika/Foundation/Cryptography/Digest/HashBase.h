/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_HashBase_h_
#define _Stroika_Foundation_Cryptography_Digest_HashBase_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <concepts>
#include <functional>

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 * 
 *  \note - this separate file exists to avoid mutual include issues - it logically would make sense to
 *          include this in Hash.h
 */

namespace Stroika::Foundation::Cryptography::Digest {

    // see https://stackoverflow.com/questions/65127936/defining-a-c20-concept-for-hash-functions
    // https://en.cppreference.com/w/cpp/utility/hash
    template <typename FUNCTION, typename HASHABLE_T>
    concept IHashFunction = std::regular_invocable<FUNCTION, HASHABLE_T> && requires (FUNCTION f, HASHABLE_T t) {
        {
            std::invoke (f, t)
        } -> std::convertible_to<size_t>;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "HashBase.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_HashBase_h_*/
