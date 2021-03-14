/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_CacheControl_h_
#define _Stroika_Foundation_IO_Network_HTTP_CacheControl_h_ 1

#include "../../../StroikaPreComp.h"

#include <optional>

#include "../../../Characters/String.h"
#include "../../../Configuration/Enumeration.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /**
     *  Support Cache-Control headers
     *      @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control
     *      @see https://tools.ietf.org/html/rfc7234 (Hypertext Transfer Protocol (HTTP/1.1): Caching)
     * 
     *  \par Example Usage
     *      \code
     *          auto cc1 = CacheControl{CacheControl::eNoStore};                                // Cache-Control: no-store
     *          auto cc2 = CacheControl{.fStoreRestriction=CacheControl::eNoStore, .fMaxAge=0}; // Cache-Control: no-store, max-age=0
     *          auto cc3 = CacheControl{.fMaxAge=1234};                                         // Cache-Control: max-age=1234
     *          auto cc3 = CacheControl{.fMaxAge=604800};                                       // Cache-Control: public, max-age=604800
     *      \endcode
     * 
     *  \par Example Usage (@see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#caching_static_assets)
     *      \code
     *          auto cc = CacheControl{.fVisibility=CacheControl::ePublic, .fImmutable=true, .fMaxAge=CacheControl::kMaximumAgeValue}; // Cache-Control: public, max-age=2147483647, immutable
     *          see kImmutable
     *      \endcode
     * 
     *  \par Example Usage (@see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#preventing_caching)
     *      \code
     *          auto cc1 = CacheControl{.fStoreRestriction=CacheControl::StoreRestriction::eNoStore};             // Cache-Control: no-store
     *          auto cc2 = CacheControl{.fStoreRestriction=CacheControl::StoreRestriction::eNoStore, .fMaxAge=0}; // Cache-Control: no-store max-age=0; like above but forces clear of existing cached item
     *          see kDisableCaching
     *      \endcode
     *
     *  \par Example Usage (Web-Services) - just a rough example cuz this will depend alot on the web-service
     *      \code
     *          auto cc1 = CacheControl{.fVisibility=CacheControl::ePrivate, .fMaxAge=Duration{30s}.As<int> ()}; // Cache-Control: private, max-age=30
     *          auto cc2 = CacheControl::kDisableCaching;
     *          // @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#requiring_revalidation
     *          auto cc3 = CacheControl{.fStoreRestriction=CacheControl::StoreRestriction::eNoCache};             // Cache-Control: no-cache
     *          auto cc3 = CacheControl{.fMaxAge=0, .fMustRevalidate=true};                                       // Cache-Control: max-age=0, must-revalidate
     *          auto cc4 = CacheControl::kMustRevalidate;
     *          auto cc5 = CacheControl::kPrivateMustRevalidate;        // ** probably best to use for most webservice calls, except those that have side-effects, and those should use kDisableCaching **
     *      \endcode
     */
    struct CacheControl {

        /**
         *  The Parse the header, SILENTLY ignorning anything in the header which is unrecognized.
         */
        static CacheControl Parse (const Characters::String& headerValue);

        /**
         *  @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#cacheability
         */
        enum class Cacheability {
            ePublic,  // The response may be stored by any cache, even if the response is normally non-cacheable
            ePrivate, // The response may be stored only by a browser's cache, even if the response is normally non-cacheable
            eNoCache, // The response may be stored by any cache, even if the response is normally non-cacheable. However, the stored response MUST always go through validation with the origin server first
            eNoStore, // The response may not be stored in any cache (can also set max-age=0 to also clear existing cache responses)

            Stroika_Define_Enum_Bounds (ePublic, eNoStore)
        };
        static constexpr Cacheability ePublic  = Cacheability::ePublic;
        static constexpr Cacheability ePrivate = Cacheability::ePrivate;
        static constexpr Cacheability eNoStore = Cacheability::eNoStore;
        static constexpr Cacheability eNoCache = Cacheability::eNoCache;

        /**
         */
        optional<Cacheability> fCacheability;

        /**
         *  The number of seconds a resource is considered fresh (**very common**)
         *  \note req fMaxAge <= kMaximumAgeValue
         */
        optional<uint32_t> fMaxAge;

        static constexpr uint32_t kMaximumAgeValue = numeric_limits<int32_t>::max (); // see https://tools.ietf.org/html/rfc7234#section-1.2.1

        /**
         */
        bool fMustRevalidate{false};

        /**
         * @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#revalidation_and_reloading
         */
        bool fImmutable{false};

        /**
         * @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#other
         */
        bool fNoTransform{false};

        /**
         * @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#other
         */
        bool fOnlyIfCached{false};

        /**
         *  Used by servers just in proxying (otherwise no reason to return something with a non-zero age)
         *  \note req fMaxAge <= kMaximumAgeValue
         */
        optional<uint32_t> fAge;

        /**
         *  overrides fMaxAge & Expires, but ONLY for shared caches(e.g. proxies)
         *  \note req fMaxAge <= kMaximumAgeValue
         */
        optional<int> fSharedMaxAge;

        /**
         *  Indicates the client will accept a stale response
         */
        struct MaxStale {
            //An optional value in seconds indicates the upper limit of staleness the client will accept.
            optional<int> fAmount;
#if __cpp_impl_three_way_comparison >= 201907
            nonvirtual strong_ordering operator<=> (const MaxStale&) const = default;
#endif
        };
        optional<MaxStale> fMaxStale;

        /**
         *  Indicates the client wants a response that will still be fresh for at least the specified number of seconds
         */
        optional<int> fMinFresh;

        /**
         *  Supported values for T include:
         *      String
         */
        template <typename T>
        T As () const;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const CacheControl&) const = default;
#endif
    public:
        /**
         *  \par Example Usage (@see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#caching_static_assets)
         *      \code
         *          kImmutable = CacheControl{.fCacheability=CacheControl::ePublic, .fMaxAge=CacheControl::kMaximumAgeValue, .fImmutable=true}; // Cache-Control: public, max-age=2147483647, immutable
         *      \endcode
         */
        static const CacheControl kImmutable;

    public:
        /**
         *  \par Example Usage (@see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#preventing_caching)
         *      \code
         *          auto cc = CacheControl{.fCacheability=CacheControl::eNoStore};             // Cache-Control: no-store
         *      \endcode
         */
        static const CacheControl kDisableCaching;

    public:
        /**
         *  \brief this means you CAN cache the value, but should revalidate each time before use (so etags can be used etc)
         * 
         *  From https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#requiring_revalidation:
         *    no-cache and max-age=0, must-revalidate indicates same meaning
         *
         *  \par Example Usage (@see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#requiring_revalidation)
         *      \code
         *          auto cc = CacheControl{.fCacheability=CacheControl::eNoCache};             // Cache-Control: no-cache
         *      \endcode
         */
        static const CacheControl kMustRevalidatePublic;

    public:
        /**
         *  \brief this means you CAN cache the value, but should revalidate each time before use (so etags can be used etc) - but it should not be re-used from user to user
         */
        static const CacheControl kMustRevalidatePrivate;
    };
    template <>
    Characters::String CacheControl::As () const;

#if __cpp_designated_initializers
    inline constexpr const CacheControl CacheControl::kImmutable{.fCacheability = CacheControl::ePublic, .fMaxAge = CacheControl::kMaximumAgeValue, .fImmutable = true};
#else
    inline constexpr const CacheControl CacheControl::kImmutable{CacheControl::ePublic, CacheControl::kMaximumAgeValue, false, true};
#endif
#if __cpp_designated_initializers
    inline constexpr const CacheControl CacheControl::kDisableCaching{.fCacheability = CacheControl::eNoStore};
#else
    inline constexpr const CacheControl CacheControl::kDisableCaching{CacheControl::eNoStore};
#endif
#if __cpp_designated_initializers
    inline constexpr const CacheControl CacheControl::kMustRevalidatePublic{.fCacheability = CacheControl::eNoCache};
#else
    inline constexpr const CacheControl CacheControl::kMustRevalidatePublic{CacheControl::eNoCache};
#endif
#if __cpp_designated_initializers
    inline constexpr const CacheControl CacheControl::kMustRevalidatePrivate{.fCacheability = CacheControl::ePrivate, .fMustRevalidate = true};
#else
    inline constexpr const CacheControl CacheControl::kMustRevalidatePrivate{CacheControl::ePrivate, nullopt, true};
#endif

#if __cpp_impl_three_way_comparison < 201907
    bool operator== (const CacheControl& lhs, const CacheControl& rhs);
    bool operator!= (const CacheControl& lhs, const CacheControl& rhs);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CacheControl.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_CacheControl_h_*/
