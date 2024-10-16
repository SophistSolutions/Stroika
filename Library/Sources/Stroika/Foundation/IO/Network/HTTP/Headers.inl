/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ********************************* HTTP::Headers ********************************
     ********************************************************************************
     */
    inline void Headers::Add (const KeyValuePair<String, String>& hrdAndValue)
    {
        Add (hrdAndValue.fKey, hrdAndValue.fValue);
    }
    inline void Headers::Add (const pair<String, String>& hrdAndValue)
    {
        Add (hrdAndValue.first, hrdAndValue.second);
    }
    inline void Headers::operator+= (const pair<String, String>& hrdAndValue)
    {
        Add (hrdAndValue);
    }
    inline void Headers::operator+= (const KeyValuePair<String, String>& hrdAndValue)
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

}
