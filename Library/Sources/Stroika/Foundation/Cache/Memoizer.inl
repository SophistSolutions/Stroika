/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Common.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ******************************** Cache::Memoizer *******************************
     ********************************************************************************
     */
    template <typename RESULT, template <qStroika_template_template_BWA (typename, typename)> class CACHE, typename... ARGS>
    Memoizer<RESULT, CACHE, ARGS...>::Memoizer (const function<RESULT (ARGS...)>& f, CACHE<tuple<ARGS...>, RESULT>&& cache)
        : fFunction_{f}
        , fCache_{forward<CACHE<tuple<ARGS...>, RESULT>> (cache)}
    {
    }
    template <typename RESULT, template <qStroika_template_template_BWA (typename, typename)> class CACHE, typename... ARGS>
    RESULT Memoizer<RESULT, CACHE, ARGS...>::operator() (ARGS... args)
    {
        // Lookup the value in the cache, and if that fails, call fFunction_ (and add that to the cache)
        return fCache_.LookupValue (make_tuple (args...), [&] (const tuple<ARGS...>& t) { return apply (fFunction_, t); });
    }

    /*
     ********************************************************************************
     ************************ Cache::Factory::Memoizer::Make ************************
     ********************************************************************************
     */
    namespace Private_ {
        template <template <typename...> typename CACHE, typename F, typename Tuple>
        struct memoizer_builder;
        template <template <typename...> typename CACHE, typename F, typename... Args>
        struct memoizer_builder<CACHE, F, std::tuple<Args...>> {
            using type = Memoizer<typename Common::FunctionTraits<F>::result_type, LRUCache, Args...>;
        };
    }
    template <template <qStroika_template_template_BWA (typename, typename)> typename CACHE, typename FUNCTION>
    auto Factory::Memoizer::Make (FUNCTION&& f)
    {
        using ArgsTuple = typename Common::FunctionTraits<FUNCTION>::args_tuple;
        typename Private_::memoizer_builder<CACHE, FUNCTION, ArgsTuple>::type m (forward<FUNCTION> (f));
        return m;
    }

}
