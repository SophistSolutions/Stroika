/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_Memoizer_inl_
#define _Stroika_Foundation_Cache_Memoizer_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Containers/Common.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ******************************** Cache::Memoizer *******************************
     ********************************************************************************
     */
    template <typename RESULT, template <typename, typename> class CACHE, typename... ARGS>
    Memoizer<RESULT, CACHE, ARGS...>::Memoizer (const function<RESULT (ARGS...)>& f, CACHE<tuple<ARGS...>, RESULT>&& cache)
        : fFunction_ (f)
        , fCache_ (forward<CACHE<tuple<ARGS...>, RESULT>> (cache))
    {
    }
    template <typename RESULT, template <typename, typename> class CACHE, typename... ARGS>
    RESULT Memoizer<RESULT, CACHE, ARGS...>::Compute (ARGS... args)
    {
        // Lookup the value in the cache, and if that fails, call fFunction_ (and add that to the cache)
        return fCache_.LookupValue (
            make_tuple (args...),
            [&] (const tuple<ARGS...>& t) {
                return apply (fFunction_, t);
            });
    }

}

#endif /*_Stroika_Foundation_Cache_Memoizer_inl_*/
