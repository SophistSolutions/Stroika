/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_Common_h_
#define _Stroika_Foundation_Cache_Common_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <functional>
#include <optional>
#include <type_traits>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/TypeHints.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Cache {

    /**
     * @brief This sentinal value can be used as the KEY type for a Cache to indicate it is un-keyed, and there is just one.
     */
    using NonKeyedKeySentinalType = void;

    /**
     * @brief A KEY is any copyable value (or the sentinal type void - indicating a keyless - single valued - cache)
     */
    template <typename T>
    concept IKey = same_as<T, NonKeyedKeySentinalType> or copyable<T>;

    /**
     * @brief does this cache have a KEY type (overwhelming YES, but sometimes handy to have 'singleton' cache, where you cache something, but just one of them)
     * 
     *  \par Example Usage:
     *      \code
     *          using Cache::SynchronizedTimedCache;
     *          // one cache of network interfaces - but dont recompute it periodically
     *          static SynchronizedTimedCache<NonKeyedKeySentinalType, Collection<NetworkInterface>> sCache_;
     *          results = sCache_.LookupValue (allowedStaleness.value_or (kDefaultItemCacheLifetime_),
     *                                  [] () -> Collection<NetworkInterface> { return CollectAllNetworkInterfaces_ (); });
     *      \endcode
     */
    template <typename KEY>
    static constexpr bool IKeyedCache = not same_as<KEY, NonKeyedKeySentinalType>;

    /**
     * @brief This sentinal value can be used as the VALUE type for a Cache to indicate it just stores ONLY the KEYS (and sometimes related time/expiration) information.
     * 
     * Also, this can be used like KeyedCollection - where the main object acts like a KEY and value at the same time (often just internally has a KEY field).
     */
    using ValuelessSentinalType = void;

    /**
     * @brief any copyable type can use used as the value, or the special sentinal type - ValuelessSentinalType, used to indicate we are just caching presence/absense of the KEY in the cache (and its expiration date).
     */
    template <typename T>
    concept IValue = same_as<T, ValuelessSentinalType> or copyable<T>;

    /**
     * @brief Some caches (rare) only support a KEY, with no associated value (the value is stored INSIDE the key itself)
     * 
     *  \par Example Usage:
     *      \code
     *      \endcode
     */
    template <typename VALUE>
    static constexpr bool IValuelessCache = same_as<VALUE, ValuelessSentinalType>;

    /**
     * @brief Check if the argument CACHE is a valid 'stroika cache' class, following its api
     * 
     *  \note ICache allows for both Valueless (IValuelessCache) and Keyless (IKeyedCache) caches, but some cache implementations may not support
     *        one or the other.
     * 
     *        Also, this API/Interface does NOT support BloomFilters, because they don't have the lookup () function
     *        (due to false positives).
     */
    template <typename CACHE, typename KEY, typename VALUE>
    concept ICache = IKey<KEY> and IValue<VALUE> and not(not IKeyedCache<KEY> and IValuelessCache<VALUE>) and
                     ( // IKeyedCache case constraints
                         (IKeyedCache<KEY> and ((IValuelessCache<VALUE> and
                                                 requires (CACHE c, KEY k) {
                                                     { c.Add (k) };
                                                     { c.Lookup (k) } -> convertible_to<optional<KEY>>;
                                                     { c.LookupValue (k, function<VALUE (KEY)>{}) } -> convertible_to<KEY>;
                                                 }) or
                                                (not IValuelessCache<VALUE> and
                                                 requires (CACHE c, KEY k, VALUE v) {
                                                     { c.Add (k, v) };
                                                     { c.Lookup (k) } -> convertible_to<optional<VALUE>>;
                                                     { c.LookupValue (k, function<VALUE (KEY)>{}) } -> convertible_to<VALUE>;
                                                 })))
                         // not IKeyedCache case constraints
                         or (not IKeyedCache<KEY> and requires (CACHE c, VALUE v) {
                                { c.Add (v) };
                                { c.Lookup () } -> convertible_to<optional<VALUE>>;
                                { c.LookupValue (function<VALUE ()>{}) } -> convertible_to<VALUE>;
                            }));

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cache_Common_h_ */
