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
     */
    struct CacheControl {

        /**
         *  The constructor overload taking headerValue, parses the header, SILENTLY ignorning anything
         *  in the header which is unrecognized.
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
         *  Used by servers just in proxying (otherwise no reason to return soemthing with a non-zero age)
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
    };
    template <>
    Characters::String CacheControl::As () const;

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
