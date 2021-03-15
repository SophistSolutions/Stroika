/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Cookie_inl_
#define _Stroika_Foundation_IO_Network_HTTP_Cookie_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ********************************** HTTP::Cookie ********************************
     ********************************************************************************
     */
    inline Cookie::Cookie (const String& name, const String& value)
        : KeyValuePair<String, String>{name, value}
    {
    }
    inline String HTTP::Cookie::ToString () const
    {
        return As<String> ();
    }
#if __cpp_impl_three_way_comparison < 201907
    inline bool operator== (const Cookie& lhs, const Cookie& rhs)
    {
        // clang-format off
        return 
            lhs.fKey == rhs.fKey 
            and lhs.fValue == rhs.fValue
            and lhs.fExpires == rhs.fExpires
            and lhs.fMaxAge == rhs.fMaxAge
            and lhs.fDomain == rhs.fDomain
            and lhs.fPath == rhs.fPath
            and lhs.fSecure == rhs.fSecure
            and lhs.fHttpOnly == rhs.fHttpOnly
            and lhs.fOtherAttributes == rhs.fOtherAttributes
            ;
        // clang-format on
    }
    inline bool operator!= (const Cookie& lhs, const Cookie& rhs)
    {
        return not(lhs == rhs);
    }
#endif

    /*
     ********************************************************************************
     ******************************** HTTP::CookieList ******************************
     ********************************************************************************
     */
    inline CookieList::CookieList (const CookieList& src)
        : CookieList{}
    {
        fCookieDetails_ = src.fCookieDetails_;
    }
    inline CookieList::CookieList (CookieList&& src)
        : CookieList{}
    {
        fCookieDetails_ = move (src.fCookieDetails_);
    }
    inline CookieList::CookieList (const Mapping<String, String>& basicCookies)
        : CookieList{}
    {
        this->cookies = basicCookies;
    }
    inline CookieList::CookieList (const Collection<Cookie>& cookieDetails)
        : CookieList{}
    {
        fCookieDetails_ = cookieDetails;
    }
    inline CookieList& CookieList::operator= (const CookieList& rhs)
    {
        this->fCookieDetails_ = rhs.fCookieDetails_;
        return *this;
    }
    inline String HTTP::CookieList::ToString () const
    {
        return EncodeForCookieHeader ();
    }
#if __cpp_impl_three_way_comparison >= 201907
    inline bool CookieList::operator== (const CookieList& rhs) const
    {
        return Traversal::Iterable<Cookie>::SequentialEqualsComparer<>{}(fCookieDetails_, rhs.fCookieDetails_);
    }
#endif
#if __cpp_impl_three_way_comparison < 201907
    inline bool operator== (const CookieList& lhs, const CookieList& rhs)
    {
        return Traversal::Iterable<Cookie>::SequentialEqualsComparer<>{}(lhs.fCookieDetails_, rhs.fCookieDetails_);
    }
    inline bool operator!= (const CookieList& lhs, const CookieList& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_Cookie_inl_*/
