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
     *      \endcode
     *
     *  \par Example Usage (static content that could change) - just a rough example cuz this will depend alot on your application
     *      \code
     *          auto cc1 = CacheControl{.fVisibility=CacheControl::ePublic, .fMaxAge=Duration{3*24h}.As<int> ()}; // Cache-Control: public, max-age=259200
     *          auto cc2 = CacheControl::kDisableCaching;
     *      \endcode
     * 
     */
    struct CacheControl {

        /**
         *  The Parse the header, SILENTLY ignorning anything in the header which is unrecognized.
         */
        static CacheControl Parse (const Characters::String& headerValue);

        /**
         */
        enum class StoreRestriction {
            eNoStore,
            eNoCache,
            eNoTransform,
            eOnlyIfCached,

            Stroika_Define_Enum_Bounds (eNoStore, eOnlyIfCached)
        };
        static constexpr StoreRestriction eNoStore      = StoreRestriction::eNoStore;
        static constexpr StoreRestriction eNoCache      = StoreRestriction::eNoCache;
        static constexpr StoreRestriction eNoTransform  = StoreRestriction::eNoTransform;
        static constexpr StoreRestriction eOnlyIfCached = StoreRestriction::eOnlyIfCached;

        /**
         */
        optional<StoreRestriction> fStoreRestriction;

        /**
         */
        enum class Visibility {
            ePublic,
            ePrivate,

            Stroika_Define_Enum_Bounds (ePublic, ePrivate)
        };
        static constexpr Visibility ePublic  = Visibility::ePublic;
        static constexpr Visibility ePrivate = Visibility::ePrivate;

        /**
         */
        optional<Visibility> fVisibility;

        /**
         */
        bool fMustRevalidate{false};

        /**
         * @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#revalidation_and_reloading
         */
        bool fImmutable{false};

        /**
         *  Used by servers just in proxying (otherwise no reason to return something with a non-zero age)
         *  \note req fMaxAge <= kMaximumAgeValue
         */
        optional<uint32_t> fAge;

        /**
         *  The number of seconds a resource is considered fresh (**very common**)
         *  \note req fMaxAge <= kMaximumAgeValue
         */
        optional<uint32_t> fMaxAge;

        static constexpr uint32_t kMaximumAgeValue = numeric_limits<int32_t>::max (); // see https://tools.ietf.org/html/rfc7234#section-1.2.1

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
         *          kImmutable = CacheControl{.fVisibility=CacheControl::ePublic, .fMaxAge=CacheControl::kMaximumAgeValue, .fImmutable=true}; // Cache-Control: public, max-age=2147483647, immutable
         *      \endcode
         */
        static const CacheControl kImmutable;

    public:
        /**
         *  \par Example Usage (@see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control#preventing_caching)
         *      \code
         *          auto cc = CacheControl{.fStoreRestriction=CacheControl::StoreRestriction::eNoStore};             // Cache-Control: no-store
         *      \endcode
         */
        static const CacheControl kDisableCaching;
    };
    template <>
    Characters::String CacheControl::As () const;

#if __cpp_designated_initializers
    inline constexpr const CacheControl CacheControl::kImmutable{.fVisibility = CacheControl::ePublic, .fImmutable = true, .fMaxAge = CacheControl::kMaximumAgeValue};
#else
    inline constexpr const CacheControl CacheControl::kImmutable{nullopt, CacheControl::ePublic, false, true, CacheControl::kMaximumAgeValue};
#endif
#if __cpp_designated_initializers
    inline constexpr const CacheControl CacheControl::kDisableCaching{.fStoreRestriction = CacheControl::StoreRestriction::eNoStore};
#else
    inline constexpr const CacheControl CacheControl::kDisableCaching{CacheControl::StoreRestriction::eNoStore};
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
