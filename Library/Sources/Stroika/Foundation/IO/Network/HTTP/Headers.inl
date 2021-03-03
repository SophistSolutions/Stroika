/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Headers_inl_
#define _Stroika_Foundation_IO_Network_HTTP_Headers_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ********************************* HTTP::Headers ********************************
     ********************************************************************************
     */
    inline void Headers::Add (const pair<String, String>& hrdAndValue)
    {
        Add (hrdAndValue.first, hrdAndValue.second);
    }
    inline void Headers::operator+= (const pair<String, String>& hrdAndValue)
    {
        Add (hrdAndValue);
    }
    inline void Headers::operator+= (const Headers& headers)
    {
        AddAll (headers);
    }
    template <>
    inline Iterable<KeyValuePair<String, String>> Headers::As () const
    {
        return As<Collection<KeyValuePair<String, String>>> ();
    }
#if __cpp_impl_three_way_comparison >= 201907
    inline bool Headers::operator== (const Headers& rhs) const
    {
        using ExtrasComparer = Collection<KeyValuePair<String, String>>::SequentialEqualsComparer<>;
        // clang-format off
        return 
            ExtrasComparer{}(fExtraHeaders_, rhs.fExtraHeaders_)
            and fCacheControl_ == rhs.fCacheControl_ 
            and fContentLength_ == rhs.fContentLength_ 
            and fContentType_ == rhs.fContentType_ 
            and fCookieList_ == rhs.fCookieList_ 
            and fDate_ == rhs.fDate_ 
            and fETag_ == rhs.fETag_ 
            and fHost_ == rhs.fHost_ 
            and fIfNoneMatch_ == rhs.fIfNoneMatch_
            and fSetCookieList_ == rhs.fSetCookieList_
            and fTransferEncoding_ == rhs.fTransferEncoding_
            and fVary_ == rhs.fVary_
            ;
        // clang-format on
    }
#endif
#if __cpp_impl_three_way_comparison < 201907
    inline bool operator== (const Headers& lhs, const Headers& rhs)
    {
        using ExtrasComparer = Collection<KeyValuePair<String, String>>::SequentialEqualsComparer<>;
        // clang-format off
        return 
            ExtrasComparer{}(lhs.fExtraHeaders_, rhs.fExtraHeaders_)
            and lhs.fCacheControl_ == rhs.fCacheControl_ 
            and lhs.fContentLength_ == rhs.fContentLength_ 
            and lhs.fContentType_ == rhs.fContentType_ 
            and lhs.fCookieList_ == rhs.fCookieList_ 
            and lhs.fDate_ == rhs.fDate_ 
            and lhs.fETag_ == rhs.fETag_ 
            and lhs.fHost_ == rhs.fHost_ 
            and lhs.fIfNoneMatch_ == rhs.fIfNoneMatch_
            and lhs.fSetCookieList_ == rhs.fSetCookieList_
            and lhs.fTransferEncoding_ == rhs.fTransferEncoding_
            and lhs.fVary_ == rhs.fVary_
            ;
        // clang-format on
    }
    inline bool operator!= (const Headers& lhs, const Headers& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_Headers_inl_*/
