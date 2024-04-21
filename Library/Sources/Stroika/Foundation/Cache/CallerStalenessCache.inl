
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Cache_CallerStalenessCache_inl_
#define _Stroika_Foundation_Cache_CallerStalenessCache_inl_ 1

#include "Stroika/Foundation/Debug/Assertions.h"

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
    inline typename CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::TimeStampType CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Ago (TimeStampDifferenceType backThisTime)
    {
        Require (backThisTime >= 0s);
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
            for (auto i = fData_.begin (); i != fData_.end ();) {
                if (i->fValue.fDataCapturedAt < t) {
                    fData_.Remove (i, &i);
                }
                else {
                    ++i;
                }
            }
        }
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
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
    template <typename K>
    inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear (const K& k)
        requires (IsKeyedCache<K>)
    {
        fData_.Remove (k);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Add (Configuration::ArgByValueType<VALUE> v)
        requires (not IsKeyedCache<KEY>)
    {
        fData_ = myVal_{move (v), GetCurrentTimestamp ()};
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K>
    inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Add (Configuration::ArgByValueType<K>     k,
                                                                    Configuration::ArgByValueType<VALUE> v, AddReplaceMode addReplaceMode)
        requires (IsKeyedCache<K>)
    {
        fData_.Add (k, myVal_{move (v), GetCurrentTimestamp ()}, addReplaceMode);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline optional<VALUE> CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (TimeStampType staleIfOlderThan) const
        requires (not IsKeyedCache<KEY>)
    {
        optional<myVal_> o = fData_;
        if (not o.has_value () or o->fDataCapturedAt < staleIfOlderThan) {
            return nullopt;
        }
        return o->fValue;
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K>
    inline optional<VALUE> CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (Configuration::ArgByValueType<K> k, TimeStampType staleIfOlderThan) const
        requires (IsKeyedCache<K>)
    {
        optional<myVal_> o = fData_.Lookup (k);
        if (not o.has_value () or o->fDataCapturedAt < staleIfOlderThan) {
            return nullopt;
        }
        return o->fValue;
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    VALUE CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::LookupValue (TimeStampType staleIfOlderThan, const function<VALUE ()>& cacheFiller)
        requires (not IsKeyedCache<KEY>)
    {
        optional<myVal_> o = fData_;
        if (not o.has_value () or o->fDataCapturedAt < staleIfOlderThan) {
            VALUE  v{cacheFiller ()}; // cache fill can take some time, so make sure we call GetCurrentTimestamp() after cachefiller
            myVal_ mv{move (v), GetCurrentTimestamp ()};
            fData_ = mv;
            return move (mv.fValue);
        }
        return o->fValue;
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename F, typename K>
    VALUE CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::LookupValue (Configuration::ArgByValueType<K> k, TimeStampType staleIfOlderThan, F&& cacheFiller)
        requires (IsKeyedCache<K> and is_invocable_r_v<VALUE, F, K>)
    {
        optional<myVal_> o = fData_.Lookup (k);
        if (not o.has_value () or o->fDataCapturedAt < staleIfOlderThan) {
            VALUE  v{cacheFiller (k)}; // cache fill can take some time, so make sure we call GetCurrentTimestamp() after cachefiller
            myVal_ mv{move (v), GetCurrentTimestamp ()};
            fData_.Add (k, mv);
            return move (mv.fValue);
        }
        return o->fValue;
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    template <typename K>
    inline VALUE CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::LookupValue (Configuration::ArgByValueType<K> k,
                                                                             TimeStampType staleIfOlderThan, const VALUE& defaultValue) const
        requires (IsKeyedCache<K>)
    {
        return Lookup (k, staleIfOlderThan).value_or (defaultValue);
    }
    template <typename KEY, typename VALUE, typename TIME_TRAITS>
    inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::clear ()
    {
        Clear ();
    }

}

#endif /*_Stroika_Foundation_Cache_CallerStalenessCache_inl_*/
