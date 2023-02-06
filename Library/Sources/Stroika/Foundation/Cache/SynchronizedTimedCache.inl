
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
     ***************** SynchronizedTimedCache<KEY, VALUE, TRAITS> *******************
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename TRAITS>
    inline SynchronizedTimedCache<KEY, VALUE, TRAITS>::SynchronizedTimedCache (const Time::Duration& minimumAllowedFreshness)
        : inherited{minimumAllowedFreshness}
    {
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline SynchronizedTimedCache<KEY, VALUE, TRAITS>::SynchronizedTimedCache (const SynchronizedTimedCache& src)
        : inherited{src.GetMinimumAllowedFreshness ()}
    {
        [[maybe_unused]] auto&& srcLock = shared_lock{src.fMutex_}; // shared locks intrinsically recursive - not needed here but good to assure no locks in between
        for (const auto& ci : src.Elements ()) {
            inherited::Add (ci.fKey, ci.fValue, ci.fFreshness);
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline Time::Duration SynchronizedTimedCache<KEY, VALUE, TRAITS>::GetMinimumAllowedFreshness () const
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::GetMinimumAllowedFreshness ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::SetMinimumAllowedFreshness (Time::Duration minimumAllowedFreshness)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::SetMinimumAllowedFreshness (minimumAllowedFreshness);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline auto SynchronizedTimedCache<KEY, VALUE, TRAITS>::Elements () const -> Traversal::Iterable<CacheElement>
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::Elements ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline optional<VALUE> SynchronizedTimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key,
                                                                               Time::DurationSecondsType* lastRefreshedAt) const
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::Lookup (key);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline auto SynchronizedTimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key,
                                                                    LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag)
        -> optional<VALUE>
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        return inherited::Lookup (key);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline auto SynchronizedTimedCache<KEY, VALUE, TRAITS>::LookupValue (typename Configuration::ArgByValueType<KEY> key,
                                                                         const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller,
                                                                         LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag,
                                                                         PurgeSpoiledDataFlagType   purgeSpoiledData) -> VALUE
    {
        /*
         *  The main reason for this class (as opposed to Syncrhonized<TimedCache>), is this logic: unlocking the shared 
         *  lock and then fetching the new value (oprionally with a write lock).
         */
        auto&& readLock = shared_lock{fMutex_}; // try shared_lock for case where present, and then lose it if we need to update object
        if (optional<VALUE> o = inherited::Lookup (key)) {
            return *o;
        }
        else {
            readLock.unlock (); // don't hold read lock, upgrade to write, and condition when we hold the write lock
            if (fHoldWriteLockDuringCacheFill) {
                // Avoid two threds calling cache for same key value at the same time
                [[maybe_unused]] auto&& newRWLock = lock_guard{fMutex_};
                VALUE                   v         = cacheFiller (key);
                inherited::Add (key, v, purgeSpoiledData); // if purgeSpoiledData must be done, do while holding lock
                return v;
            }
            else {
                // Avoid needlessly blocking lookups (shared_lock above) until after we've filled the cache (typically slow)
                // and keep it to minimum logically required (inherited add).
                VALUE v = cacheFiller (key);
                Add (key, v, purgeSpoiledData); // OUR top-level implementation does right thing with locking and purgeSpoiledData, so can use that.
                return v;
            }
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::Add (typename Configuration::ArgByValueType<KEY>   key,
                                                                 typename Configuration::ArgByValueType<VALUE> result,
                                                                 TimedCacheSupport::PurgeSpoiledDataFlagType   purgeSpoiledData)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        // NOTE - COULD handle purgeSpoiledData directly here, and use two lock_guards, so other callers get a chance before purge loop
        // but this is probably better as fewer lock calls and not likely doing a purge loop anyhow...
        inherited::Add (key, result, purgeSpoiledData);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::Add (typename Configuration::ArgByValueType<KEY> key,
                                                                 typename Configuration::ArgByValueType<VALUE> result, Time::Duration freshAsOf)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Add (key, result, freshAsOf);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::Remove (typename Configuration::ArgByValueType<KEY> key)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Remove (key);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::clear ()
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::clear ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::PurgeSpoiledData ()
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::PurgeSpoiledData ();
    }

}

#endif /*_Stroika_Foundation_Cache_SynchronizedTimedCache_inl_*/
