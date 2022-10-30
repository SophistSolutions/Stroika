
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
        : inherited{}
    {
        [[maybe_unused]] auto&& srcLock = shared_lock{src.fMutex_}; // shared locks intrinsically recursive - not needed here but good to assure no locks in between
        [[maybe_unused]] auto&& lock    = lock_guard{fMutex_};
        inherited::SetMinimumAllowedFreshness (src.GetMinimumAllowedFreshness ());
        for (const auto& ci : src.GetElements ()) {
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
    inline auto SynchronizedTimedCache<KEY, VALUE, TRAITS>::GetElements () const -> Traversal::Iterable<CacheElement>
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::GetElements ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline optional<VALUE> SynchronizedTimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key, Time::DurationSecondsType* lastRefreshedAt) const
    {
        [[maybe_unused]] auto&& lock = shared_lock{fMutex_};
        return inherited::Lookup (key);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline auto SynchronizedTimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key, LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag) -> optional<VALUE>
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        return inherited::Lookup (key);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline auto SynchronizedTimedCache<KEY, VALUE, TRAITS>::LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller, LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag) -> VALUE
    {
        auto&& lock = lock_guard{fMutex_};
        return inherited::LookupValue (key, cacheFiller, successfulLookupRefreshesAcceesFlag);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void SynchronizedTimedCache<KEY, VALUE, TRAITS>::Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result, TimedCacheSupport::PurgeSpoiledDataFlagType purgeSpoiledData)
    {
        [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
        inherited::Add (key, result, purgeSpoiledData);
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
