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
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fCacheControl_;
    }
    inline void Headers::SetCacheControl (const optional<CacheControl>& cacheControl)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fCacheControl_ = cacheControl;
    }
    inline optional<uint64_t> Headers::GetContentLength () const
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fContentLength_;
    }
    inline void Headers::SetContentLength (const optional<uint64_t>& contentLength)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fContentLength_ = contentLength;
    }
    inline optional<InternetMediaType> Headers::GetContentType () const
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fContentType_;
    }
    inline void Headers::SetContentType (const optional<InternetMediaType>& contentType)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fContentType_ = contentType;
    }
    inline optional<ETag> Headers::GetETag () const
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fETag_;
    }
    inline void Headers::SetETag (const optional<ETag>& etag)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fETag_ = etag;
    }

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_Headers_inl_*/
