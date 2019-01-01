/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_FunctionalApplication_inl_
#define _Stroika_Foundation_Traversal_FunctionalApplication_inl_

#include "../Containers/Sequence.h"
#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     ******************** Traversal::DirectPushMapEngine ****************************
     ********************************************************************************
     */
    template <typename IN_T, typename OUT_T>
    Iterable<OUT_T> DirectPushMapEngine::Map (const Iterable<IN_T>& from, const function<OUT_T (IN_T)>& do2Each)
    {
        Containers::Sequence<OUT_T> result;
        for (IN_T i : from) {
            // unsure if we update in place, or create a new container? May need traits param to define how todo this!
            result.Append (do2Each (i));
        }
        return result;
    }
    template <typename IN_T, typename OUT_T>
    OUT_T DirectPushMapEngine::Reduce (const Iterable<IN_T>& from, const function<OUT_T (IN_T, OUT_T)>& do2Each, OUT_T memo)
    {
        OUT_T result = memo;
        for (IN_T i : from) {
            result = do2Each (i, result);
        }
        return result;
    }
    template <typename IN_OUT_T>
    Iterable<IN_OUT_T> DirectPushMapEngine::Filter (const Iterable<IN_OUT_T>& from, const function<bool(IN_OUT_T)>& includeTest)
    {
        Containers::Sequence<IN_OUT_T> result;
        for (IN_OUT_T i : from) {
            if (includeTest (i)) {
                result.Append (i);
            }
        }
        return result;
    }
    template <typename IN_OUT_T>
    optional<IN_OUT_T> DirectPushMapEngine::Find (const Iterable<IN_OUT_T>& from, const function<bool(IN_OUT_T)>& thatPassesThisTest)
    {
        for (IN_OUT_T i : from) {
            if (thatPassesThisTest (i)) {
                return i;
            }
        }
        return nullopt;
    }

    /*
     ********************************************************************************
     ******************** Traversal::FunctionalApplicationContext *******************
     ********************************************************************************
     */
    template <typename T, typename MAPPER_ENGINE>
    inline FunctionalApplicationContext<T, MAPPER_ENGINE>::FunctionalApplicationContext (Iterable<T> i, MAPPER_ENGINE m)
        : inherited (i)
        , fMappingEngine_ (m)
    {
    }
    template <typename T, typename MAPPER_ENGINE>
    template <typename OUT_T>
    inline FunctionalApplicationContext<OUT_T, MAPPER_ENGINE> FunctionalApplicationContext<T, MAPPER_ENGINE>::Map (const function<OUT_T (T)>& do2Each)
    {
        return FunctionalApplicationContext<OUT_T, MAPPER_ENGINE> (fMappingEngine_.Map (inherited (*this), do2Each), fMappingEngine_);
    }
    template <typename T, typename MAPPER_ENGINE>
    template <typename OUT_T>
    inline OUT_T FunctionalApplicationContext<T, MAPPER_ENGINE>::Reduce (const function<OUT_T (T, OUT_T)>& do2Each, OUT_T memo)
    {
        return fMappingEngine_.Reduce (inherited (*this), do2Each, memo);
    }
    template <typename T, typename MAPPER_ENGINE>
    template <typename INOUT_T>
    inline FunctionalApplicationContext<INOUT_T, MAPPER_ENGINE> FunctionalApplicationContext<T, MAPPER_ENGINE>::Filter (const function<bool(INOUT_T)>& includeTest)
    {
        return FunctionalApplicationContext<INOUT_T, MAPPER_ENGINE> (fMappingEngine_.Filter (inherited (*this), includeTest), fMappingEngine_);
    }
    template <typename T, typename MAPPER_ENGINE>
    template <typename INOUT_T>
    optional<INOUT_T> FunctionalApplicationContext<T, MAPPER_ENGINE>::Find (const function<bool(INOUT_T)>& thatPassesThisTest)
    {
        return fMappingEngine_.Find (inherited (*this), thatPassesThisTest);
    }

}

#endif /* _Stroika_Foundation_Traversal_FunctionalApplication_inl_ */
