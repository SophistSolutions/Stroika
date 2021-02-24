/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Digester_inl_
#define _Stroika_Foundation_Cryptography_Digest_Digester_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "ResultTypes.h"

namespace Stroika::Foundation::Cryptography::Digest {

    /*
     ********************************************************************************
     *********************** Digester<ALGORITHM, RETURN_TYPE> ***********************
     ********************************************************************************
     */
    template <typename ALGORITHM, typename RETURN_TYPE>
    auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const Streams::InputStream<std::byte>::Ptr& from) const -> ReturnType
    {
        Algorithm::DigesterAlgorithm<ALGORITHM> ctx;
        while (true) {
            byte   buf[32 * 1024];
            size_t n = from.Read (std::begin (buf), std::end (buf));
            Assert (n <= sizeof (buf));
            if (n == 0) {
                break;
            }
            ctx.Write (std::begin (buf), std::begin (buf) + n);
        }
        if constexpr (is_same_v<RETURN_TYPE, typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>) {
            return ctx.Complete ();
        }
        else {
            return ConvertResult<RETURN_TYPE> (ctx.Complete ());
        }
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const std::byte* from, const std::byte* to) const -> ReturnType
    {
        Algorithm::DigesterAlgorithm<ALGORITHM> ctx;
        ctx.Write (from, to);
        if constexpr (is_same_v<RETURN_TYPE, typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>) {
            return ctx.Complete ();
        }
        else {
            return ConvertResult<RETURN_TYPE> (ctx.Complete ());
        }
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const BLOB& from) const -> ReturnType
    {
        return this->operator() (from.begin (), from.end ());
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_Digester_inl_*/
