/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_CacheControl_inl_
#define _Stroika_Foundation_IO_Network_HTTP_CacheControl_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ****************************** HTTP::CacheControl ******************************
     ********************************************************************************
     */
    inline Characters::String CacheControl::ToString () const
    {
        return As<Characters::String> ();
    }

#if __cpp_impl_three_way_comparison < 201907
    inline bool operator== (const CacheControl::MaxStale& lhs, const CacheControl::MaxStale& rhs)
    {
        return lhs.fAmount == rhs.fAmount;
    }
    inline bool operator== (const CacheControl& lhs, const CacheControl& rhs)
    {
        return lhs.fCacheability == rhs.fCacheability and lhs.fMaxAge == rhs.fMaxAge and lhs.fMustRevalidate == rhs.fMustRevalidate and lhs.fImmutable == rhs.fImmutable and lhs.fNoTransform == rhs.fNoTransform and lhs.fOnlyIfCached == rhs.fOnlyIfCached and lhs.fAge == rhs.fAge and lhs.fSharedMaxAge == rhs.fSharedMaxAge and lhs.fMaxStale == rhs.fMaxStale and lhs.fMinFresh == rhs.fMinFresh;
    }
    inline bool operator!= (const CacheControl& lhs, const CacheControl& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<IO::Network::HTTP::CacheControl::Cacheability> DefaultNames<IO::Network::HTTP::CacheControl::Cacheability>::k{
        EnumNames<IO::Network::HTTP::CacheControl::Cacheability>::BasicArrayInitializer{{
            {IO::Network::HTTP::CacheControl::Cacheability::ePublic, L"public"},
            {IO::Network::HTTP::CacheControl::Cacheability::ePrivate, L"private"},
            {IO::Network::HTTP::CacheControl::Cacheability::eNoCache, L"no-cache"},
            {IO::Network::HTTP::CacheControl::Cacheability::eNoStore, L"no-store"},
        }}};
}

#endif /*_Stroika_Foundation_IO_Network_HTTP_CacheControl_inl_*/
