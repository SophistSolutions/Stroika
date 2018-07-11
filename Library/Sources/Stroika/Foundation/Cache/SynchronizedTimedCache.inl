
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Cache_SynchronizedTimedCache_inl_
#define _Stroika_Foundation_Cache_SynchronizedTimedCache_inl_ 1

namespace Stroika::Foundation {
    namespace Cache {

        /*
         ********************************************************************************
         ************** SynchronizedTimedCache<KEY, VALUE, TRAITS> **********************
         ********************************************************************************
         */
        template <typename KEY, typename VALUE, typename TRAITS>
        SynchronizedTimedCache<KEY, VALUE, TRAITS>::SynchronizedTimedCache (Time::DurationSecondsType timeoutInSeconds)
            : inherited (timeoutInSeconds)
        {
        }
        template <typename KEY, typename VALUE, typename TRAITS>
        VALUE SynchronizedTimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller)
        {
            shared_lock<shared_timed_mutex> lock{fMutex_};
            if (optional<VALUE> o = inherited::Lookup (key)) {
                return *o;
            }
            else {
                lock.unlock ();
                if (fHoldWriteLockDuringCacheFill) {
                    // Avoid two threds calling cache for same key value at the same time
                    lock_guard<shared_timed_mutex> newRWLock{fMutex_};
                    VALUE                          v = cacheFiller (key);
                    this->Add (key, v);
                }
                else {
                    // Avoid needlessly blocking lookups (shared_lock above) until after we've filled the cache (typically slow)
                    // and keep it to minimum logically required (inherited add).
                    VALUE                          v = cacheFiller (key);
                    lock_guard<shared_timed_mutex> newRWLock{fMutex_};
                    this->Add (key, v);
                }
                return v;
            }
        }
    }
}

#endif /*_Stroika_Foundation_Cache_SynchronizedTimedCache_inl_*/
