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
     *********************************** HTTP::Headers ******************************
     ********************************************************************************
     */
    inline optional<CacheControl> Headers::GetCacheControl () const
    {
        return fCacheControl_;
    }
    inline void Headers::SetCacheControl (const optional<CacheControl>& cacheControl)
    {
        fCacheControl_ = cacheControl;
    }
    inline optional<uint64_t> Headers::GetContentLength () const
    {
        return fContentLength_;
    }
    inline void Headers::SetContentLength (const optional<uint64_t>& contentLength)
    {
        fContentLength_ = contentLength;
    }
    inline optional<InternetMediaType> Headers::GetContentType () const
    {
        return fContentType_;
    }
    inline void Headers::SetContentType (const optional<InternetMediaType>& contentType)
    {
        fContentType_ = contentType;
    }
    inline optional<ETag> Headers::GetETag () const
    {
        return fETag_;
    }
    inline void Headers::SetETag (const optional<ETag>& etag)
    {
        fETag_ = etag;
    }

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_Headers_inl_*/
