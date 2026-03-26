/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_Common_h_
#define _Stroika_Foundation_Cache_Common_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <type_traits>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/TypeHints.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Cryptography/Digest/Hash.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Cache {

    /**
     * @brief A KEY is any copyable value (or the sentinal type void - indicating a keyless - single valued - cache)
     */
    template <typename T>
    concept IKey = same_as<T, void> or copyable<T>;

    /**
     * @brief any copyable type can use used as the value, with 'void' being a special sentinal type, used to indicate we are just caching presence/absense of the KEY in the cache (and its expiration date).
     */
    template <typename T>
    concept IValue = same_as<T, void> or copyable<T>;

    /**
     * @brief does this cache have a KEY type (overwhelming YES, but sometimes handy to have 'singleton' cache, where you cache something, but just one of them)
     * 
     *  \par Example Usage:
     *      \code
     *          using Cache::SynchronizedCallerStalenessCache;
     *          // one cache of network interfaces - but dont recompute it periodically
     *          static SynchronizedCallerStalenessCache<void, Collection<NetworkInterface>> sCache_;
     *          results = sCache_.LookupValue (allowedStaleness.value_or (kDefaultItemCacheLifetime_),
     *                                  [] () -> Collection<NetworkInterface> { return CollectAllNetworkInterfaces_ (); });
     *      \endcode
     */
    template <typename KEY>
    static constexpr bool IKeyedCache = not same_as<KEY, void>;

    /**
     * @brief A KEY is any copyable value (or the sentinal type void - indicating a keyless - single valued - cache)
     * 
     *  Supports KEY=void means single value cache, VALUE=void means like a Set, not a Map<>.
     */
    template <typename CACHE, typename KEY, typename VALUE>
    concept ICache = IKey<KEY> and IValue<VALUE> and
                         // IKeyedCache
                         (IKeyedCache<KEY> and
                          requires (CACHE c, KEY k, VALUE v) {
                              { c.Add (k, v) };
                              { c.Lookup (k) } -> convertible_to<optional<VALUE>>;
                              { c.LookupValue (k, function<VALUE (KEY)>{}) } -> convertible_to<VALUE>;
                          })
                     // not IKeyedCache
                     or (not IKeyedCache<KEY> and requires (CACHE c, VALUE v) {
                            { c.Add (v) };
                            { c.Lookup () } -> convertible_to<optional<VALUE>>;
                            { c.LookupValue (function<VALUE ()>{}) } -> convertible_to<VALUE>;
                        });

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cache_Common_h_ */
