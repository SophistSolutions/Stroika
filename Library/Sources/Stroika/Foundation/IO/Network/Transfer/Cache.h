/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Cache_h_
#define _Stroika_Foundation_IO_Network_Transfer_Cache_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Foundation/IO/Network/Transfer/Request.h"
#include "Stroika/Foundation/IO/Network/Transfer/Response.h"

/**
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  An HTTP cache for IO::Network::Transfer::Connection: IRep is the interface any cache implements (in memory, on disk, ...),
 *  and a Ptr to one goes in Connection::Options::fCache. Element is a cached response, parsed, with the freshness rules
 *  (RFC 9111) an implementation can share.
 *
 *  New () makes Stroika's own: in memory, least recently used out first, configured by Options.
 */

namespace Stroika::Foundation::IO::Network::Transfer::Cache {

    /**
     *  \brief Options for the cache New () makes: Stroika's in-memory cache, of whole responses, by URL
     */
    struct Options {
        /**
         *  The most responses kept; past it, the least recently used one goes.
         */
        size_t fCacheSize{101};

        /**
         *  How long a response stays fresh when it does not say itself (no max-age, no Expires). Unset, such a response is stale
         *  at once: each use asks the server again - conditionally, where the response has an ETag or Last-Modified.
         */
        optional<Time::Duration> fDefaultResourceTTL;

        /**
         */
        static const inline String kCachedResultHeaderDefault{"X-Stroika-Cached-Result"sv};

        /**
         *  The header added to each response the cache answers (with an empty value), so the caller can tell a Response came
         *  from the cache. Unset, none is added.
         */
        optional<String> fCachedResultHeader{kCachedResultHeaderDefault};
    };

    /**
     *  This is the basic element of data stored in the cache for any cached URL. Some implementations may store more or less,
     *  But this provides the rough outline of what is expected to cache, and utility apis to extract the cache control policy data.
     */
    struct Element {
    public:
        Element () = default;
        Element (const Response& response);

    public:
        /**
         */
        nonvirtual Mapping<String, String> GetCombinedHeaders () const;

    public:
        /**
         *  Check - based on incoming http headers - whether this resource is cachable.
         */
        nonvirtual bool IsCachable () const;

    public:
        /**
         *  return nullopt if unknown (so invokes some sort of default)
         *
         *  Invalid doesn't mean you must throw away. Just that you must validate (conditional get) to see if this can be re-used.
         *  This returns the amount of time you dont even need to bother revalidating with a conditional get.
         *
         *  From the response's headers, as RFC 9111 says: Cache-Control no-cache makes it already past (revalidate each use),
         *  whatever else they say; else fFreshUntil; else fExpiresDefault.
         */
        nonvirtual optional<Time::DateTime> IsValidUntil () const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    public:
        optional<String> fETag; // without its quotes
        /**
         *  The ETag came as a weak one (W/"...") - sent back as such, in If-None-Match, which compares weakly (RFC 9110, 13.1.2).
         */
        bool                     fETagIsWeak{false};
        optional<Time::DateTime> fExpires;
        /**
         *  When the response goes stale, by this machine's clock (RFC 9111, 4.2): from when it was received, its freshness lifetime -
         *  its max-age, else its Expires less its Date - less the age it came with - its Age, or the time since its Date, whichever is
         *  more. Missing where it has neither max-age nor Expires. So a response a proxy held a while, or a server clock that runs
         *  fast or slow, does not stretch it.
         */
        optional<Time::DateTime>                  fFreshUntil;
        optional<Time::DateTime>                  fLastModified;
        optional<Containers::Set<String>>         fCacheControl; // parsed/split rep
        Memory::BLOB                              fBody;         // no point in a cache entry without this
        optional<DataExchange::InternetMediaType> fContentType;
        Mapping<String, String>                   fOtherHeaders; // headers not captured/extracted above
        /**
         *  When a cache's default TTL (Options::fDefaultResourceTTL) has this response go stale - used only where the response says
         *  nothing itself (no max-age, no Expires). Set by the cache, not from the response.
         */
        optional<Time::DateTime> fExpiresDefault;
    };

    /**
     *  Used internally by Connection::Rep subclasses to call the Cache::Rep API. The reason for this is we want some information
     *  snapshotted from before the request to be used after the request (when someone else may have deleted the item from the cache).
     */
    struct EvalContext {
        EvalContext () = default;

        optional<Element> fCachedElement;
        optional<URI>     fFullURI;
    };

    /**
     */
    struct IRep {

        virtual ~IRep () = default;

        /**
         *  was called BeforeGet - but can decide internally - and callers can decide to only use on get
         */
        virtual optional<Response> OnBeforeFetch (EvalContext* context, const URI& schemeAndAuthority, Request* request) noexcept = 0;

        /**
         * replaces response value with right answer on 304, and caches results from successful fetch calls.
         * was called AfterGet
         */
        virtual void OnAfterFetch (const EvalContext& context, Response* response) noexcept = 0;

        /**
         * if not cleared, external cache can be re-used
         */
        virtual void ClearCache () = 0;

        /**
         */
        virtual optional<Element> Lookup (const URI& url) const = 0;
    };

    /**
     *  \brief A reference to an HTTP cache (an IRep) - what Connection::Options::fCache holds, so every Connection made from those
     *         options shares the one cache
     *
     *  A shared_ptr<IRep>, as a Ptr is: copies refer to the same cache; nullptr means no cache. Get one from New (), or
     *  implement IRep - a disk cache, say - and make a Ptr of that:
     *      \code
     *          Connection::Options options;
     *          options.fCache     = Cache::New ({.fDefaultResourceTTL = 300s});
     *          Connection::Ptr c1 = Connection::New (options); // c1 and c2 share one cache
     *          Connection::Ptr c2 = Connection::New (options);
     *          ...
     *          options.fCache->ClearCache ();
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>
     *          The Ptr, as a shared_ptr; the cache, as its IRep says - New's is internally synchronized, so connections
     *          on any number of threads may share it.
     */
    struct Ptr : shared_ptr<Cache::IRep> {};

    /**
     *  \brief Make an HTTP cache - Stroika's in-memory one - to share among Connections, by assigning it to their
     *         Connection::Options::fCache (see Ptr)
     *
     *  The cache is internally synchronized: connections on any number of threads may share it.
     *
     *  \note This is Stroika's default implementation of an HTTP cache: it keeps responses in memory, in an LRU cache of
     *        Options::fCacheSize entries. It is not the only one possible: any subclass of IRep works with the rest of this
     *        API - an external, disk-based cache, say - and you use yours instead by putting a Ptr to it in
     *        Connection::Options::fCache.
     */
    Ptr New (const Options& options = {});

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Cache.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Cache_h_*/
