/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
    inline bool operator== (const CacheControl::MaxAge& lhs, const CacheControl::MaxAge& rhs)
    {
        return lhs.fValue == rhs.fValue;
    }
    inline bool operator== (const CacheControl& lhs, const CacheControl& rhs)
    {
        return lhs.fStoreRestriction == rhs.fStoreRestriction and lhs.fVisibility == rhs.fVisibility and lhs.fMustRevalidate == rhs.fMustRevalidate and lhs.fAge == rhs.fAge and lhs.fMaxAge == rhs.fMaxAge and lhs.fSharedMaxAge == rhs.fSharedMaxAge and lhs.fMaxStale == rhs.fMaxStale and lhs.fMinFresh == rhs.fMinFresh;
    }
    inline bool operator!= (const CacheControl& lhs, const CacheControl& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<IO::Network::HTTP::CacheControl::StoreRestriction> DefaultNames<IO::Network::HTTP::CacheControl::StoreRestriction>::k{
        EnumNames<IO::Network::HTTP::CacheControl::StoreRestriction>::BasicArrayInitializer{{
            {IO::Network::HTTP::CacheControl::StoreRestriction::eNoStore, L"no-store"},
            {IO::Network::HTTP::CacheControl::StoreRestriction::eNoCache, L"no-cache"},
            {IO::Network::HTTP::CacheControl::StoreRestriction::eNoTransform, L"no-transform"},
            {IO::Network::HTTP::CacheControl::StoreRestriction::eOnlyIfCached, L"only-if-cached"},
        }}};
    template <>
    constexpr EnumNames<IO::Network::HTTP::CacheControl::Visibility> DefaultNames<IO::Network::HTTP::CacheControl::Visibility>::k{
        EnumNames<IO::Network::HTTP::CacheControl::Visibility>::BasicArrayInitializer{{
            {IO::Network::HTTP::CacheControl::Visibility::ePublic, L"public"},
            {IO::Network::HTTP::CacheControl::Visibility::ePrivate, L"private"},
        }}};
}

#endif /*_Stroika_Foundation_IO_Network_HTTP_CacheControl_inl_*/
