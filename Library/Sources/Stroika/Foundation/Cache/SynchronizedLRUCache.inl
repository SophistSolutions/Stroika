
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Cache_SynchronizedLRUCache_inl_
#define _Stroika_Foundation_Cache_SynchronizedLRUCache_inl_ 1

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     * SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE> **
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    template <typename... ARGS>
    SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::SynchronizedLRUCache (ARGS... args)
        : inherited{forward<ARGS> (args)...} // allow implicit conversions so int converted to size_t like with direct call to LRUCache
    {
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::SynchronizedLRUCache (const SynchronizedLRUCache& src)
        : inherited{src.GetMaxCacheSize (), src.GetKeyEqualsCompareFunction (), src.GetHashTableSize (), src.GetKeyHashFunction ()}
        , fHoldWriteLockDuringCacheFill{src.fHoldWriteLockDuringCacheFill}
    {
        for (auto i : src.Elements ()) {
            Add (i.fKey, i.fValue);
        }
        // @todo - no way to copy the stats
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline size_t SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::GetMaxCacheSize () const
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::GetMaxCacheSize ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::SetMaxCacheSize (size_t maxCacheSize)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::SetMaxCacheSize (maxCacheSize);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline auto SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::GetStats () const -> StatsType
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::GetStats ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline auto SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::GetKeyEqualsCompareFunction () const -> KeyEqualsCompareFunctionType
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::GetKeyEqualsCompareFunction ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline auto SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::GetHashTableSize () const -> size_t
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::GetHashTableSize ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline auto SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::GetKeyHashFunction () const -> KEY_HASH_FUNCTION
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::GetKeyHashFunction ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::clear ()
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::clear ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::clear (typename Configuration::ArgByValueType<KEY> key)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::clear (key);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::clear (function<bool (typename Configuration::ArgByValueType<KEY>)> clearPredicate)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::clear (clearPredicate);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline optional<VALUE> SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::Lookup (typename Configuration::ArgByValueType<KEY> key) const
    {
        // Avoid issue with Lookup updating the stats object - if there is one - with whichKindOfLocker
        using whichKindOfLocker      = conditional_t<is_same_v<Statistics::Stats_Null, STATS_TYPE>, shared_lock<decltype (fMutex_)>, lock_guard<decltype (fMutex_)>>;
        [[maybe_unused]] auto&& lock = whichKindOfLocker{fMutex_};
        return inherited::Lookup (key);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    VALUE SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& valueFetcher)
    {
        /*
         *  The main reason for this class, is this logic: unlocking the shared lock and then fetching the new value (with a write lock).
         */
        // Avoid issue with Lookup updating the stats object - if there is one - with whichKindOfLocker
        using whichKindOfLocker = conditional_t<is_same_v<Statistics::Stats_Null, STATS_TYPE>, shared_lock<decltype (fMutex_)>, unique_lock<decltype (fMutex_)>>;
        auto&& lock             = whichKindOfLocker{fMutex_};
        if (optional<VALUE> o = inherited::Lookup (key)) {
            return *o;
        }
        else {
            lock.unlock ();
            if (fHoldWriteLockDuringCacheFill) {
                // Avoid two threds calling cache filler for same key value at the same time
                [[maybe_unused]] auto&& newRWLock = lock_guard{fMutex_};
                VALUE                   v         = valueFetcher (key);
                inherited::Add (key, v);
                return v;
            }
            else {
                // Avoid needlessly blocking lookups (shared_lock above) until after we've filled the cache (typically slow)
                // and keep it to minimum logically required (inherited add).
                VALUE v = valueFetcher (key);
                Add (key, v);
                return v;
            }
        }
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> value)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Add (key, value);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline Containers::Mapping<KEY, VALUE> SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::Elements () const
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::Elements ();
    }

}

#endif /*_Stroika_Foundation_Cache_SynchronizedLRUCache_inl_*/
