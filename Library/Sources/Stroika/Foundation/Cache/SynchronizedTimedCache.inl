
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
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::SetTimeout (Time::DurationSecondsType timeoutInSeconds)
    {
        auto&& lock = lock_guard{fMutex_};
        inherited::SetTimeout (timeoutInSeconds);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline optional<VALUE> SynchronizedTimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key)
    {
        auto&& lock = lock_guard{fMutex_};
        return inherited::Lookup (key);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    VALUE SynchronizedTimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller)
    {
        auto&& lock = lock_guard{fMutex_};
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
                return v;
            }
            else {
                // Avoid needlessly blocking lookups (shared_lock above) until after we've filled the cache (typically slow)
                // and keep it to minimum logically required (inherited add).
                VALUE  v         = cacheFiller (key);
                auto&& newRWLock = lock_guard{fMutex_};
                this->Add (key, v);
                return v;
            }
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result)
    {
        auto&& lock = lock_guard{fMutex_};
        inherited::Add (key, result);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::Remove (typename Configuration::ArgByValueType<KEY> key)
    {
        auto&& lock = lock_guard{fMutex_};
        inherited::Remove (key);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::clear ()
    {
        auto&& lock = lock_guard{fMutex_};
        inherited::clear ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::DoBookkeeping ()
    {
        auto&& lock = lock_guard{fMutex_};
        inherited::DoBookkeeping ();
    }
}

#endif /*_Stroika_Foundation_Cache_SynchronizedTimedCache_inl_*/
