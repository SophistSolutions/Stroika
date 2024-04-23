/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
    inline CookieList& CookieList::operator= (CookieList&& rhs)
    {
        this->fCookieDetails_ = move (rhs.fCookieDetails_);
        return *this;
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
    inline bool CookieList::operator== (const CookieList& rhs) const
    {
        return Traversal::Iterable<Cookie>::SequentialEqualsComparer<>{}(fCookieDetails_, rhs.fCookieDetails_);
    }

}
