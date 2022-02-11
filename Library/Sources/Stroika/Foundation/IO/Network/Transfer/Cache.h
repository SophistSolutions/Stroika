/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Cache_h_
#define _Stroika_Foundation_IO_Network_Transfer_Cache_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/ToString.h"
#include "../../../Containers/Set.h"
#include "../../../Time/DateTime.h"
#include "../../../Time/Duration.h"

#include "Request.h"
#include "Response.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    /**
     *  This cache is a generic HTTP fetch cache API, allowing users of the IO::Transfer::Connection code to
     *  use different cache implementations (e.g. one storing to disk, and one to RAM).
     *
     *  It is hopefully done in such a way that you can do alot of code sharing among these implementations,
     *  but do not need to.
     *
     *  This Cache class also comes with an opaque, one-size-fits all (who they kidding) implementation, with a
     *  handful of configuration options.
     */
    class Cache {
    public:
        struct Rep;

    public:
        struct Ptr;

    public:
        struct Element;

    public:
        struct EvalContext;

    public:
        /**
         */
        struct DefaultOptions {
            DefaultOptions () = default;

            optional<size_t> fCacheSize;

            /**
             *  if not specified via expires header, or max-age etc...
             */
            optional<Time::Duration> fDefaultResourceTTL;

            /**
             */
            static const inline String kCachedResultHeaderDefault{L"X-Stroika-Cached-Result"sv};

            /**
             *  This header will always appear in cached results. If missing it will not be generated. It defaults to kCachedResultHeaderDefault.
             */
            optional<String> fCachedResultHeader{kCachedResultHeaderDefault};
        };

    public:
        /**
         *  Creates a default-styled HTTP cache object. Note - you still must associate this cache object with
         *  any cache connections you create (typically by assigning it to the Connection creation factory)
         *
         *  NOTE - though the smart ptr Ptr is not fully re-entrant, the letter class created by CreateDefault is fully
         *  internally synchronized. This means you can re-use it with multiple connections and run those connection requests from as
         *  many threads as desired.
         *
         *  @todo - redo with default argument, but messed up on gcc/clang/not sure why -- LGP 2019-06-27
         *  @todo - maybe rename New ()
         */
        static Ptr CreateDefault ();
        static Ptr CreateDefault (const DefaultOptions& options);
    };

    /**
     *  This is the basic element of data stored in the cache for any cached URL. Some implementations may store more or less,
     *  But this provides the rough outline of what is expected to cache, and utility apis to extract the cache control policy data.
     */
    struct Cache::Element {
    public:
        Element () = default;
        Element (const Response& response);

    public:
        virtual ~Element () = default;

    public:
        /**
         */
        nonvirtual Mapping<String, String> GetCombinedHeaders () const;

    public:
        /**
         *  Check - based on incoming http headers - whether this resoure is cachable.
         */
        virtual bool IsCachable () const;

    public:
        /**
         *  return nullopt if unknown (so invokes some sort of default)
         *
         *  Invalid doesn't mean you must throw away. Just that you must validate (conditional get) to see if this can be re-used.
         *  This returns the amount of time you dont even need to bother revalidating with a conditional get.
         */
        virtual optional<Time::DateTime> IsValidUntil () const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    public:
        optional<String>                          fETag;
        optional<Time::DateTime>                  fExpires;
        optional<Time::DateTime>                  fExpiresDueToMaxAge;
        optional<Time::DateTime>                  fLastModified;
        optional<Containers::Set<String>>         fCacheControl; // parsed/split rep
        Memory::BLOB                              fBody;         // no point in a cache entry without this
        optional<DataExchange::InternetMediaType> fContentType;
        Mapping<String, String>                   fOtherHeaders; // headers not captured/extracted above
    };

    /**
     *  Used internally by Connection::Rep subclasses to call the Cache::Rep API. The reason for this is we want some information
     *  snapshotted from before the request to be used after the request (when someone else may have deleted the item from the cache).
     */
    struct Cache::EvalContext {
        EvalContext () = default;

        optional<Element> fCachedElement;
        optional<URI>     fFullURI;
    };

    /**
     */
    struct Cache::Rep {

        virtual ~Rep () = default;

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
     */
    struct Cache::Ptr : shared_ptr<Cache::Rep> {
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Cache.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Cache_h_*/
