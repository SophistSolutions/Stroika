
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Cache_CallerStalenessCache_inl_
#define _Stroika_Foundation_Cache_CallerStalenessCache_inl_  1

#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {


            /*
             ********************************************************************************
             ********************** CallerStalenessCache_Traits_DEFAULT *********************
             ********************************************************************************
             */
            inline  CallerStalenessCache_Traits_DEFAULT::TimeStampType   CallerStalenessCache_Traits_DEFAULT::GetCurrentTimestamp ()
            {
                return Time::GetTickCount ();
            }


            /*
             ********************************************************************************
             ****************** CallerStalenessCache<KEY,VALUE,TIME_TRAITS> *****************
             ********************************************************************************
             */
            template    <typename   KEY, typename VALUE, typename TIME_TRAITS>
            inline  typename    CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::TimeStampType CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::GetCurrentTimestamp ()
            {
                return TIME_TRAITS::GetCurrentTimestamp ();
            }
            template    <typename   KEY, typename VALUE, typename TIME_TRAITS>
            nonvirtual  void    CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::ClearOlderThan (TimeStampType t)
            {
                for (auto i = fMap_.begin (); i != fMap_.end (); ++i) {
                    if (i->fValue.fDataCapturedAt < t) {
                        fMap_.erase (i);
                    }
                }
            }
            template    <typename   KEY, typename VALUE, typename TIME_TRAITS>
            inline  void    CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear (KEY k)
            {
                fMap_.Remove (k);
            }
            template    <typename   KEY, typename VALUE, typename TIME_TRAITS>
            inline  void    CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear ()
            {
                fMap_.clear();
            }
            template    <typename   KEY, typename VALUE, typename TIME_TRAITS>
            VALUE CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup (KEY k, TimeStampType staleIfOlderThan, const std::function<VALUE()>& cacheFiller)
            {
                Memory::Optional<myVal_> o = fMap_.Lookup (k);
                if (o.IsMissing () or o->fDataCapturedAt < staleIfOlderThan) {
                    myVal_ mv (cacheFiller (), GetCurrentTimestamp());
                    fMap_.Add (k, mv);
                    return mv.fValue;
                }
                return o->fValue;
            }


        }
    }
}


#endif  /*_Stroika_Foundation_Cache_CallerStalenessCache_inl_*/
