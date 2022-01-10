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
