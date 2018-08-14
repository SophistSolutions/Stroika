
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Cache_SynchronizedTimedCache_inl_
#define _Stroika_Foundation_Cache_SynchronizedTimedCache_inl_ 1

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ************** SynchronizedTimedCache<KEY, VALUE, TRAITS> **********************
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename TRAITS>
    inline SynchronizedTimedCache<KEY, VALUE, TRAITS>::SynchronizedTimedCache (Time::DurationSecondsType timeoutInSeconds)
        : inherited (timeoutInSeconds)
    {
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    VALUE SynchronizedTimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller)
    {
        auto&& lock = shared_lock{fMutex_};
        if (optional<VALUE> o = inherited::Lookup (key)) {
            return *o;
        }
        else {
            lock.unlock ();
            if (fHoldWriteLockDuringCacheFill) {
                // Avoid two threds calling cache for same key value at the same time
                auto&& newRWLock = lock_guard{fMutex_};
                VALUE  v         = cacheFiller (key);
                this->Add (key, v);
            }
            else {
                // Avoid needlessly blocking lookups (shared_lock above) until after we've filled the cache (typically slow)
                // and keep it to minimum logically required (inherited add).
                VALUE  v         = cacheFiller (key);
                auto&& newRWLock = lock_guard{fMutex_};
                this->Add (key, v);
            }
            return v;
        }
    }

}

#endif /*_Stroika_Foundation_Cache_SynchronizedTimedCache_inl_*/
