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
    Memoizer<RESULT, CACHE, ARGS...>::Memoizer (function<RESULT (ARGS...)> f, function<unsigned int(ARGS...)> hash)
        : Memoizer (f, hash, 101)
    {
    }
    template <typename RESULT, template <typename, typename> class CACHE, typename... ARGS>
    Memoizer<RESULT, CACHE, ARGS...>::Memoizer (function<RESULT (ARGS...)> f, function<unsigned int(ARGS...)> hash, [[maybe_unused]] size_t size)
        : fFunction_ (f)
    {
    }
    namespace {
        template <int...>
        struct seq {
        };

        template <int N, int... S>
        struct gens : gens<N - 1, N - 1, S...> {
        };

        template <int... S>
        struct gens<0, S...> {
            typedef seq<S...> type;
        };

        template <typename RESULT, typename... Args>
        struct save_it_for_later {
            std::tuple<Args...>        params;
            function<RESULT (Args...)> func;

            RESULT delayed_dispatch ()
            {
                return callFunc (typename gens<sizeof...(Args)>::type ());
            }

            template <int... S>
            RESULT callFunc (seq<S...>)
            {
                return func (std::get<S> (params)...);
            }
        };
    }
    template <typename RESULT, template <typename, typename> class CACHE, typename... ARGS>
    RESULT Memoizer<RESULT, CACHE, ARGS...>::Compute (ARGS... args)
    {
#if 0
        if (optional<RESULT> o = fCache_.Lookup (make_tuple (args...))) {
            return *o;
        }
        else {
            RESULT r = fFunction_ (args...);
            fCache_.Add (make_tuple (args...), r);
            return r;
        }
#elif 0
        save_it_for_later<RESULT, ARGS...> saved = {make_tuple (args...), fFunction_};
        return saved.delayed_dispatch ();
#elif 1
        save_it_for_later<RESULT, ARGS...> saved = {make_tuple (args...), fFunction_};
        return fCache_.LookupValue (
            make_tuple (args...),
            [&](const tuple<ARGS...>& t) { return saved.delayed_dispatch (); });
#else

        return fCache_.LookupValue (
            make_tuple (args...),
            [=](const tuple<ARGS...>& t) { return fFunction_ (typename gens<sizeof...(args)>::type ()); });
#endif
    }
}
#endif /*_Stroika_Foundation_Cache_Memoizer_inl_*/
