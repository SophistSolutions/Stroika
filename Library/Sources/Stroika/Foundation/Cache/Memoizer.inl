/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_Memoizer_inl_
#define _Stroika_Foundation_Cache_Memoizer_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Containers/Common.h"
#include "../Cryptography/Digest/Algorithm/Jenkins.h"
#include "../Cryptography/Hash.h"
#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ******************************** Cache::Memoizer *******************************
     ********************************************************************************
     */
    template <typename RESULT, template <typename, typename> class CACHE, typename... ARGS>
    Memoizer<RESULT, CACHE, ARGS...>::Memoizer (const function<RESULT (ARGS...)>& f, const function<unsigned int(ARGS...)>& hash)
        : Memoizer (f, hash, 101)
    {
    }
    template <typename RESULT, template <typename, typename> class CACHE, typename... ARGS>
    Memoizer<RESULT, CACHE, ARGS...>::Memoizer (const function<RESULT (ARGS...)>& f, function<unsigned int(ARGS...)> hash, [[maybe_unused]] size_t size)
        : fFunction_ (f)
    {
    }
    namespace PRIVATE_ {
        // @todo - Find a cleaner simpler way to exlode the tuple and call by its members...
        template <int...>
        struct seq_ {
        };
        template <int N, int... S>
        struct gens_ : gens_<N - 1, N - 1, S...> {
        };
        template <int... S>
        struct gens_<0, S...> {
            typedef seq_<S...> type;
        };
        template <typename RESULT, typename... Args>
        struct CallWithExplodedTuple_ {
            tuple<Args...>             params;
            function<RESULT (Args...)> func;
            template <int... S>
            RESULT callFunc (seq_<S...>)
            {
                return func (get<S> (params)...);
            }
        };
    }
    template <typename RESULT, template <typename, typename> class CACHE, typename... ARGS>
    RESULT Memoizer<RESULT, CACHE, ARGS...>::Compute (ARGS... args)
    {
        return fCache_.LookupValue (
            make_tuple (args...),
            [&](const tuple<ARGS...>& t) {
                using namespace Cache::PRIVATE_;
                return CallWithExplodedTuple_<RESULT, ARGS...>{t, fFunction_}.callFunc (typename gens_<sizeof...(args)>::type ());
            });
    }
}
#endif /*_Stroika_Foundation_Cache_Memoizer_inl_*/
