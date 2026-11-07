/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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

namespace Stroika::Foundation::IO::Network::HTTP {
    constexpr Common::EnumNameText EnumeratorName (CacheControl::Cacheability e, Common::EnumNameText byRule)
    {
        switch (e) {
            case CacheControl::Cacheability::ePublic:
                return "public";
            case CacheControl::Cacheability::ePrivate:
                return "private";
            case CacheControl::Cacheability::eNoCache:
                return "no-cache";
            case CacheControl::Cacheability::eNoStore:
                return "no-store";
            default:
                return byRule;
        }
    }
}
