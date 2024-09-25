/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ******** SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS> *************
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::ClearOlderThan (TimeStampType t)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::ClearOlderThan (t);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear ()
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Clear ();
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear (Common::ArgByValueType<K1> k)
        requires (IsKeyedCache<K1>)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Clear (k);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Add (Common::ArgByValueType<VALUE> v)
        requires (not IsKeyedCache<KEY>)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Add (v);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Add (Common::ArgByValueType<K> k, Common::ArgByValueType<VALUE> v,
                                                                                AddReplaceMode addReplaceMode)
        requires (IsKeyedCache<K>)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Add (k, v, addReplaceMode);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline optional<VALUE> SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (TimeStampType staleIfOlderThan) const
        requires (not IsKeyedCache<KEY>)
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::Lookup (staleIfOlderThan);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K>
    inline optional<VALUE> SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (Common::ArgByValueType<K> k, TimeStampType staleIfOlderThan) const
        requires (IsKeyedCache<K>)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        return inherited::Lookup (k, staleIfOlderThan);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline VALUE SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::LookupValue (TimeStampType staleIfOlderThan, const function<VALUE ()>& cacheFiller)
        requires (not IsKeyedCache<KEY>)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        return inherited::LookupValue (staleIfOlderThan, cacheFiller);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename F, typename K>
    inline VALUE SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::LookupValue (Common::ArgByValueType<K> k,
                                                                                         TimeStampType staleIfOlderThan, F&& cacheFiller)
        requires (IsKeyedCache<KEY> and is_invocable_r_v<VALUE, F, KEY>)
    {
        /*
         *  The main reason for this class, is this logic: unlocking the shared lock and then fetching the new value (with a write lock).
         */
        auto&& lock = shared_lock{fMutex_};
        if (optional<VALUE> o = inherited::Lookup (k, staleIfOlderThan)) {
            return *o;
        }
        else {
            lock.unlock ();
            if (fHoldWriteLockDuringCacheFill) {
                // Avoid two threads calling cache filler for same key value at the same time
                [[maybe_unused]] auto&& newRWLock = lock_guard{fMutex_};
                VALUE                   v         = cacheFiller (k);
                inherited::Add (k, v);
                return v;
            }
            else {
                // Avoid needlessly blocking lookups (shared_lock above) until after we've filled the cache (typically slow)
                // and keep it to minimum logically required (inherited add).
                VALUE v = cacheFiller (k);
                Add (k, v);
                return v;
            }
        }
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K>
    inline VALUE SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::LookupValue (Common::ArgByValueType<K> k, TimeStampType staleIfOlderThan,
                                                                                         const VALUE& defaultValue) const
        requires (IsKeyedCache<KEY>)
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_}; // ignore fHoldWriteLockDuringCacheFill since this is always fast; shared cuz doesn't update cache
        return inherited::LookupValue (k, staleIfOlderThan, defaultValue);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::clear ()
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::clear ();
    }

}
