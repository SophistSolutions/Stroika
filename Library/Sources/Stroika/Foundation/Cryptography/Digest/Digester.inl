/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Digester_inl_
#define _Stroika_Foundation_Cryptography_Digest_Digester_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Memory/StackBuffer.h"

#include "ResultTypes.h"

namespace Stroika::Foundation::Cryptography::Digest {

    /*
     ********************************************************************************
     ****************** IncrementalDigester<ALGORITHM, RETURN_TYPE> *****************
     ********************************************************************************
     */
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline void IncrementalDigester<ALGORITHM, RETURN_TYPE>::Write (const std::byte* from, const std::byte* to)
    {
        Require (not fCompleted_);
        Require ((from == nullptr and to == nullptr) or (from != nullptr and from <= to));
        fDigesterAlgorithm_.Write (from, to);
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline void IncrementalDigester<ALGORITHM, RETURN_TYPE>::Write (span<const std::byte> from)
    {
        this->Write (from.data (), from.data () + from.size ());
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
    template <typename TRIVIALLY_COPYABLE_T, enable_if_t<is_trivially_copyable_v<TRIVIALLY_COPYABLE_T>>*>
    void IncrementalDigester<ALGORITHM, RETURN_TYPE>::Write (const Traversal::Iterable<TRIVIALLY_COPYABLE_T>& from)
    {
        for (auto ci : from) {
            this->Write (&ci, &ci + 1);
        }
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline auto IncrementalDigester<ALGORITHM, RETURN_TYPE>::Complete () -> ReturnType
    {
        if constexpr (qDebug) {
            Require (not this->fCompleted_);
            this->fCompleted_ = true;
        }
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
        Require ((from == nullptr and to == nullptr) or (from != nullptr and from <= to));
        return Digest::ComputeDigest<ALGORITHM, RETURN_TYPE> (from, to);
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline auto Digester<ALGORITHM, RETURN_TYPE>::operator() (span<const std::byte> from) const -> ReturnType
    {
        return Digest::ComputeDigest<ALGORITHM, RETURN_TYPE> (from.data (), from.data () + from.size ());
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const BLOB& from) const -> ReturnType
    {
        return Digest::ComputeDigest<ALGORITHM, RETURN_TYPE> (from);
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    template <typename TRIVIALLY_COPYABLE_T, enable_if_t<is_trivially_copyable_v<TRIVIALLY_COPYABLE_T>>*>
    auto Digester<ALGORITHM, RETURN_TYPE>::operator() (const Traversal::Iterable<TRIVIALLY_COPYABLE_T>& from) const -> ReturnType
    {
        // copy to inline stack buffer, and that can be passed as array to other overloads
        Memory::StackBuffer<TRIVIALLY_COPYABLE_T> buf{from.begin (), from.end ()};
        return Digest::ComputeDigest<ALGORITHM, RETURN_TYPE> (reinterpret_cast<const std::byte*> (buf.begin ()),
                                                              reinterpret_cast<const std::byte*> (buf.end ()));
    }

    /*
     ********************************************************************************
     ****************** ComputeDigest<ALGORITHM, RETURN_TYPE> ***********************
     ********************************************************************************
     */
    template <typename ALGORITHM, typename RETURN_TYPE>
    RETURN_TYPE ComputeDigest (const std::byte* from, const std::byte* to)
    {
        Require ((from == nullptr and to == nullptr) or (from != nullptr and from <= to));
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
    inline RETURN_TYPE ComputeDigest (span<const std::byte> from)
    {
        return ComputeDigest<ALGORITHM, RETURN_TYPE> (from.data (), from.data () + from.size ());
    }
    template <typename ALGORITHM, typename RETURN_TYPE>
    inline RETURN_TYPE ComputeDigest (const BLOB& from)
    {
        return ComputeDigest<ALGORITHM, RETURN_TYPE> (from.begin (), from.end ());
    }
    template <typename ALGORITHM, typename TRIVIALLY_COPYABLE_T, typename RETURN_TYPE, enable_if_t<is_trivially_copyable_v<TRIVIALLY_COPYABLE_T>>*>
    RETURN_TYPE ComputeDigest (const Traversal::Iterable<TRIVIALLY_COPYABLE_T>& from)
    {
        // copy to inline stack buffer, and that can be passed as array to other overloads
        Memory::StackBuffer<TRIVIALLY_COPYABLE_T> buf{from.begin (), from.end ()};
        return ComputeDigest<ALGORITHM, RETURN_TYPE> (reinterpret_cast<const std::byte*> (buf.begin ()),
                                                      reinterpret_cast<const std::byte*> (buf.end ()));
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_Digester_inl_*/
