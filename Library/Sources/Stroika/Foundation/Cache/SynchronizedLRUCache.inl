
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
        : inherited (forward<ARGS> (args)...)
    {
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline size_t SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::GetMaxCacheSize () const
    {
        auto&& lock = shared_lock{fMutex_};
        return inherited::GetMaxCacheSize ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::SetMaxCacheSize (size_t maxCacheSize)
    {
        auto&& lock = lock_guard{fMutex_};
        inherited::SetMaxCacheSize (maxCacheSize);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline auto SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::GetStats () const -> StatsType
    {
        auto&& lock = shared_lock{fMutex_};
        return inherited::GetStats ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::clear ()
    {
        auto&& lock = lock_guard{fMutex_};
        inherited::clear ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::clear (typename Configuration::ArgByValueType<KEY> key)
    {
        auto&& lock = lock_guard{fMutex_};
        inherited::clear (key);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::clear (function<bool(typename Configuration::ArgByValueType<KEY>)> clearPredicate)
    {
        auto&& lock = lock_guard{fMutex_};
        inherited::clear (clearPredicate);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline optional<VALUE> SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::Lookup (typename Configuration::ArgByValueType<KEY> key) const
    {
        auto&& lock = shared_lock{fMutex_};
        return inherited::Lookup (key);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    VALUE SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& valueFetcher)
    {
        auto&& lock = shared_lock{fMutex_};
        if (optional<VALUE> o = inherited::Lookup (key)) {
            return *o;
        }
        else {
            lock.unlock ();
            if (fHoldWriteLockDuringCacheFill) {
                // Avoid two threds calling cache filler for same key value at the same time
                auto&& newRWLock = lock_guard{fMutex_};
                VALUE  v         = valueFetcher (key);
                inherited::Add (key, v);
            }
            else {
                // Avoid needlessly blocking lookups (shared_lock above) until after we've filled the cache (typically slow)
                // and keep it to minimum logically required (inherited add).
                Add (key, valueFetcher (key));
            }
            return v;
        }
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> value)
    {
        auto&& lock = lock_guard{fMutex_};
        inherited::Add (key, value);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline Containers::Mapping<KEY, VALUE> SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::Elements () const
    {
        auto&& lock = shared_lock{fMutex_};
        return inherited::Lookup (key);
    }

}

#endif /*_Stroika_Foundation_Cache_SynchronizedLRUCache_inl_*/
