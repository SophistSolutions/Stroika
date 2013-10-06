/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_MapReduce_inl_
#define _Stroika_Foundation_Traversal_MapReduce_inl_

#include    "../Debug/Assertions.h"
#include    "../Containers/Sequence.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
             ********************************************************************************
             ******************** Traversal::DirectPushMapEngine ****************************
             ********************************************************************************
             */
            template    <typename IN_T, typename OUT_T>
            Iterable<OUT_T>     DirectPushMapEngine::Map (const Iterable<IN_T>& from, const function<OUT_T(IN_T)>& do2Each) const
            {
                Containers::Sequence<OUT_T>  result;
                for (IN_T i : from) {
                    // unsure if we update in place, or create a new container? May need traits param to define how todo this!
                    result.Append (do2Each (i));
                }
                return result;
            }
            template    <typename IN_T, typename OUT_T>
            OUT_T                   DirectPushMapEngine::Reduce (const Iterable<IN_T>& from, const function<OUT_T(IN_T, OUT_T)>& do2Each, OUT_T memo) const
            {
                OUT_T    result  =   memo;
                for (IN_T i : from) {
                    result = do2Each (i, result);
                }
                return result;
            }
            template    <typename T>
            Iterable<T>         DirectPushMapEngine::Filter (const Iterable<T>& from, const function<bool(T)>& includeTest) const
            {
                Containers::Sequence<T>  result;
                for (T i : from) {
                    if (includeTest (i)) {
                        result.Append (i);
                    }
                }
                return result;
            }


            /*
             ********************************************************************************
             ******************** Traversal::FunctionApplicationContext *********************
             ********************************************************************************
             */
            template    <typename T, typename MAPPER>
            inline  FunctionApplicationContext<T, MAPPER>::FunctionApplicationContext (Iterable<T> i, MAPPER m)
                : inherited (i)
                , fMappingEngine (m)
            {
            }
            template    <typename T, typename MAPPER>
            template    <typename OUT_T>
            inline  FunctionApplicationContext<OUT_T, MAPPER>   FunctionApplicationContext<T, MAPPER>::Map (const function<OUT_T(T)>& do2Each) const
            {
                return FunctionApplicationContext<OUT_T, MAPPER>  (fMappingEngine.Map (inherited (*this), do2Each), fMappingEngine);
            }
            template    <typename T, typename MAPPER>
            template    <typename OUT_T>
            inline  T                                           FunctionApplicationContext<T, MAPPER>::Reduce (const function<OUT_T(T, OUT_T)>& do2Each, OUT_T memo) const
            {
                return  fMappingEngine.Reduce (inherited (*this), do2Each, memo);
            }
            template    <typename T, typename MAPPER>
            inline  FunctionApplicationContext<T, MAPPER>       FunctionApplicationContext<T, MAPPER>::Filter (const function<bool(T)>& includeTest) const
            {
                return FunctionApplicationContext<T>  (fMappingEngine.Filter (inherited (*this), includeTest), fMappingEngine);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_MapReduce_inl_ */
