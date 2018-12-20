
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Cache_CallerStalenessCache_inl_
#define _Stroika_Foundation_Cache_CallerStalenessCache_inl_ 1

#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ********************** CallerStalenessCache_Traits_DEFAULT *********************
     ********************************************************************************
     */
    inline CallerStalenessCache_Traits_DEFAULT::TimeStampType CallerStalenessCache_Traits_DEFAULT::GetCurrentTimestamp ()
    {
        return Time::GetTickCount ();
    }

    /*
     ********************************************************************************
     ****************** CallerStalenessCache<KEY,VALUE,TIME_TRAITS> *****************
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline typename CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::TimeStampType CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::GetCurrentTimestamp ()
    {
        return TIME_TRAITS::GetCurrentTimestamp ();
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline typename CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::TimeStampType CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Ago (TimeStampType backThisTime)
    {
        Require (backThisTime >= 0);
        return GetCurrentTimestamp () - backThisTime;
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::ClearOlderThan (TimeStampType t)
    {
        if constexpr (is_same_v<void, KEY>) {
            if (fData_.has_value () and fData_.fDataCapturedAt < t) {
                fData_ = nullopt;
            }
        }
        else {
            for (auto i = fData_.begin (); i != fData_.end (); ++i) {
                if (i->fValue.fDataCapturedAt < t) {
                    fData_.erase (i);
                }
            }
        }
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1>
    inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear ()
    {
        if constexpr (is_same_v<void, KEY>) {
            fData_ = nullopt;
        }
        else {
            fData_.clear ();
        }
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<not is_same_v<K1, void>>*>
    inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear (K1 k)
    {
        fData_.Remove (k);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<is_same_v<void, K1>>*>
    inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Add (VALUE v)
    {
        fData_ = myVal_ (move (v), GetCurrentTimestamp ());
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<not is_same_v<void, K1>>*>
    inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Add (K1 k, VALUE v)
    {
        fData_.Add (k, myVal_ (move (v), GetCurrentTimestamp ()));
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<is_same_v<void, K1>>*>
    inline optional<VALUE> CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (TimeStampType staleIfOlderThan) const
    {
        optional<myVal_> o = fData_;
        if (not o.has_value () or o->fDataCapturedAt < staleIfOlderThan) {
            return nullopt;
        }
        return o->fValue;
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<is_same_v<void, K1>>*>
    VALUE CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (TimeStampType staleIfOlderThan, const function<VALUE ()>& cacheFiller)
    {
        optional<myVal_> o = fData_;
        if (not o.has_value () or o->fDataCapturedAt < staleIfOlderThan) {
            myVal_ mv (cacheFiller (), GetCurrentTimestamp ());
            fData_ = mv;
            return move (mv.fValue);
        }
        return o->fValue;
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<not is_same_v<void, K1>>*>
    inline optional<VALUE> CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (K1 k, TimeStampType staleIfOlderThan) const
    {
        optional<myVal_> o = fData_.Lookup (k);
        if (not o.has_value () or o->fDataCapturedAt < staleIfOlderThan) {
            return nullopt;
        }
        return o->fValue;
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<not is_same_v<void, K1>>*>
    VALUE CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (K1 k, TimeStampType staleIfOlderThan, const function<VALUE ()>& cacheFiller)
    {
        optional<myVal_> o = fData_.Lookup (k);
        if (not o.has_value () or o->fDataCapturedAt < staleIfOlderThan) {
            myVal_ mv (cacheFiller (), GetCurrentTimestamp ());
            fData_.Add (k, mv);
            return move (mv.fValue);
        }
        return o->fValue;
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K1, enable_if_t<not is_same_v<void, K1>>*>
    inline VALUE CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (K1 k, TimeStampType staleIfOlderThan, const VALUE& defaultValue)
    {
        return Lookup (k, staleIfOlderThan, [defaultValue]() { return defaultValue; });
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::clear ()
    {
        Clear ();
    }

}

#endif /*_Stroika_Foundation_Cache_CallerStalenessCache_inl_*/
