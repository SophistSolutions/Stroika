/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Cache_h_
#define _Stroika_Foundation_IO_Network_Transfer_Cache_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/ToString.h"
#include "../../../Containers/Set.h"
#include "../../../Time/DateTime.h"

#include "Request.h"
#include "Response.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *
 * TODO:
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    /**
     *  This cache is a generic API, allowing users of the IO::Transfer::Connection code to
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
         *  Creates a default-styled HTTP cache object. Note - you still must associate this cache object with
         *  any cache connections you create (typically by assigning it to the Connection creation factory)
         *
         *  NOTE - thoguh the smart ptr Ptr is not fully re-entrant, the letter class created by CreateDefault is fully
         *  internally synchonized. This means you can re-use it with multiple connections and run those connection requests from as
         *  many threads as desired.
         */
        static Ptr CreateDefault ();
    };

    /**
     *  This is the basic element of data stored in the cache for any cached URL.
     */
    struct Cache::Element {
    public:
        Element () = default;
        Element (const Response& response);

        /**
         */
    public:
        nonvirtual Mapping<String, String> GetCombinedHeaders () const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    public:
        optional<String>                          fETag;
        optional<Time::DateTime>                  fExpires;
        optional<Time::DateTime>                  fLastModified;
        optional<Containers::Set<String>>         fCacheControl; // parsed/split rep
        Memory::BLOB                              fBody;         // no point in a cache entry without this
        optional<DataExchange::InternetMediaType> fContentType;
        Mapping<String, String>                   fOtherHeaders; // headers not captured/extracted above
    };

    /**
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
        virtual optional<Response> OnBeforeFetch (EvalContext* context, const URI& schemeAndAuthority, Request* request) = 0;

        /**
         * replaces response value with right answer on 304, and caches results from successful fetch calls.
         * was called AfterGet
         */
        virtual void OnAfterFetch (const EvalContext& context, Request* request, Response* response) = 0;

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
