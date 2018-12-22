
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Cache_SynchronizedCallerStalenessCache_inl_
#define _Stroika_Foundation_Cache_SynchronizedCallerStalenessCache_inl_ 1

#include "../Debug/Assertions.h"

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
    template <typename K1>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear ()
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Clear ();
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<IsKeyedCache<K1>>*>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear (K1 k)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Clear (k);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<not IsKeyedCache<K1>>*>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Add (VALUE v)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Add (v);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<IsKeyedCache<K1>>*>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Add (K1 k, VALUE v)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Add (k, v);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<not IsKeyedCache<K1>>*>
    inline optional<VALUE> SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (TimeStampType staleIfOlderThan) const
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::Lookup (staleIfOlderThan);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<not IsKeyedCache<K1>>*>
    inline VALUE SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (TimeStampType staleIfOlderThan, const function<VALUE ()>& cacheFiller)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        return inherited::Lookup (staleIfOlderThan, cacheFiller);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<IsKeyedCache<K1>>*>
    inline optional<VALUE> SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (K1 k, TimeStampType staleIfOlderThan) const
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        return inherited::Lookup (k, staleIfOlderThan);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename F, typename K1, enable_if_t<IsKeyedCache<K1> and is_invocable_r_v<VALUE, F, K1>>*>
    inline VALUE SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (K1 k, TimeStampType staleIfOlderThan, F cacheFiller)
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
    template <typename K1, enable_if_t<IsKeyedCache<K1>>*>
    inline VALUE SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (K1 k, TimeStampType staleIfOlderThan, const VALUE& defaultValue) const
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_}; // ignore fHoldWriteLockDuringCacheFill since this is always fast; shared cuz doesn't update cache
        return inherited::Lookup (k, staleIfOlderThan, defaultValue);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline void SynchronizedCallerStalenessCache<KEY, VALUE, TIME_TRAITS>::clear ()
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::clear ();
    }

}

#endif /*_Stroika_Foundation_Cache_SynchronizedCallerStalenessCache_inl_*/
