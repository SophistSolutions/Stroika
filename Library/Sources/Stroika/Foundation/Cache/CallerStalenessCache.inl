
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Cache_CallerStalenessCache_inl_
#define _Stroika_Foundation_Cache_CallerStalenessCache_inl_ 1

#include "../Debug/Assertions.h"

namespace Stroika::Foundation {
    namespace Cache {

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
            for (auto i = fMap_.begin (); i != fMap_.end (); ++i) {
                if (i->fValue.fDataCapturedAt < t) {
                    fMap_.erase (i);
                }
            }
        }
        template <typename KEY, typename VALUE, typename TIME_TRAITS>
        inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear (KEY k)
        {
            fMap_.Remove (k);
        }
        template <typename KEY, typename VALUE, typename TIME_TRAITS>
        inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear ()
        {
            fMap_.clear ();
        }
        template <typename KEY, typename VALUE, typename TIME_TRAITS>
        inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Add (KEY k, VALUE v)
        {
            fMap_.Add (k, myVal_ (move (v), GetCurrentTimestamp ()));
        }
        template <typename KEY, typename VALUE, typename TIME_TRAITS>
        optional<VALUE> CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (KEY k, TimeStampType staleIfOlderThan)
        {
            optional<myVal_> o = fMap_.Lookup (k);
            if (not o.has_value () or o->fDataCapturedAt < staleIfOlderThan) {
                return nullopt;
            }
            return o->fValue;
        }
        template <typename KEY, typename VALUE, typename TIME_TRAITS>
        VALUE CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (KEY k, TimeStampType staleIfOlderThan, const std::function<VALUE ()>& cacheFiller)
        {
            optional<myVal_> o = fMap_.Lookup (k);
            if (not o.has_value () or o->fDataCapturedAt < staleIfOlderThan) {
                myVal_ mv (cacheFiller (), GetCurrentTimestamp ());
                fMap_.Add (k, mv);
                return move (mv.fValue);
            }
            return o->fValue;
        }
        template <typename KEY, typename VALUE, typename TIME_TRAITS>
        inline VALUE CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (KEY k, TimeStampType staleIfOlderThan, const VALUE& defaultValue)
        {
            return Lookup (k, staleIfOlderThan, [defaultValue]() { return defaultValue; });
        }
        template <typename KEY, typename VALUE, typename TIME_TRAITS>
        inline void CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::clear ()
        {
            Clear ();
        }
    }
}

#endif /*_Stroika_Foundation_Cache_CallerStalenessCache_inl_*/
