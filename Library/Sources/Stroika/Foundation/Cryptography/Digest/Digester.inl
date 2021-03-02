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
     ******************* IncrementalDigester<ALGORITHM, RETURN_TYPE> ****************
     ********************************************************************************
     */
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline void IncrementalDigester<ALGORITHM, RETURN_TYPE>::Write (const std::byte* start, const std::byte* end)
    {
        Require (not fCompleted_);
        fDigesterAlgorithm_.Write (start, end);
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline void IncrementalDigester<ALGORITHM, RETURN_TYPE>::Write (const BLOB& from)
    {
        this->Write (from.begin (), from.end ());
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline void IncrementalDigester<ALGORITHM, RETURN_TYPE>::Write (const Streams::InputStream<std::byte>::Ptr& from)
    {
        while (true) {
            byte   buf[32 * 1024];
            size_t n = from.Read (std::begin (buf), std::end (buf));
            Assert (n <= sizeof (buf));
            if (n == 0) {
                break;
            }
            this->Write (std::begin (buf), std::begin (buf) + n);
        }
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline auto IncrementalDigester<ALGORITHM, RETURN_TYPE>::Complete () -> ReturnType
    {
#if qDebug
        Require (not fCompleted_);
        fCompleted_ = true;
#endif
        if constexpr (is_same_v<RETURN_TYPE, typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>) {
            return fDigesterAlgorithm_.Complete ();
        }
        else {
            return ConvertResult<RETURN_TYPE> (fDigesterAlgorithm_.Complete ());
        }
    }

    /*
     ********************************************************************************
     *********************** Digester<ALGORITHM, RETURN_TYPE> ***********************
     ********************************************************************************
     */
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const Streams::InputStream<std::byte>::Ptr& from) const -> ReturnType
    {
        return Digest::ComputeDigest<ALGORITHM, RETURN_TYPE> (from);
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const std::byte* from, const std::byte* to) const -> ReturnType
    {
        return Digest::ComputeDigest<ALGORITHM, RETURN_TYPE> (from, to);
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const BLOB& from) const -> ReturnType
    {
        return Digest::ComputeDigest<ALGORITHM, RETURN_TYPE> (from);
    }

    /*
     ********************************************************************************
     ****************** ComputeDigest<ALGORITHM, RETURN_TYPE> ***********************
     ********************************************************************************
     */
    template <typename ALGORITHM, typename RETURN_TYPE>
    RETURN_TYPE ComputeDigest (const std::byte* from, const std::byte* to)
    {
        IncrementalDigester<ALGORITHM> ctx;
        ctx.Write (from, to);
        if constexpr (is_same_v<RETURN_TYPE, typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>) {
            return ctx.Complete ();
        }
        else {
            return ConvertResult<RETURN_TYPE> (ctx.Complete ());
        }
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    RETURN_TYPE ComputeDigest (const Streams::InputStream<std::byte>::Ptr& from)
    {
        IncrementalDigester<ALGORITHM> ctx;
        ctx.Write (from);
        if constexpr (is_same_v<RETURN_TYPE, typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>) {
            return ctx.Complete ();
        }
        else {
            return ConvertResult<RETURN_TYPE> (ctx.Complete ());
        }
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline RETURN_TYPE ComputeDigest (const BLOB& from)
    {
        return ComputeDigest<ALGORITHM, RETURN_TYPE> (from.begin (), from.end ());
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_Digester_inl_*/
