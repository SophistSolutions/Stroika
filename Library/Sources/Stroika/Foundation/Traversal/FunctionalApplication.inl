/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_FunctionalApplication_inl_
#define _Stroika_Foundation_Traversal_FunctionalApplication_inl_

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
            Iterable<OUT_T>     DirectPushMapEngine::Map (const Iterable<IN_T>& from, const function<OUT_T(IN_T)>& do2Each)
            {
                Containers::Sequence<OUT_T>  result;
                for (IN_T i : from) {
                    // unsure if we update in place, or create a new container? May need traits param to define how todo this!
                    result.Append (do2Each (i));
                }
                return result;
            }
            template    <typename IN_T, typename OUT_T>
            OUT_T                   DirectPushMapEngine::Reduce (const Iterable<IN_T>& from, const function<OUT_T(IN_T, OUT_T)>& do2Each, OUT_T memo)
            {
                OUT_T    result  =   memo;
                for (IN_T i : from) {
                    result = do2Each (i, result);
                }
                return result;
            }
            template    <typename T>
            Iterable<T>         DirectPushMapEngine::Filter (const Iterable<T>& from, const function<bool(T)>& includeTest)
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
             ******************** Traversal::FunctionalApplicationContext *******************
             ********************************************************************************
             */
            template    <typename T, typename MAPPER>
            inline  FunctionalApplicationContext<T, MAPPER>::FunctionalApplicationContext (Iterable<T> i, MAPPER m)
                : inherited (i)
                , fMappingEngine_ (m)
            {
            }
            template    <typename T, typename MAPPER>
            template    <typename OUT_T>
            inline  FunctionalApplicationContext<OUT_T, MAPPER>   FunctionalApplicationContext<T, MAPPER>::Map (const function<OUT_T(T)>& do2Each)
            {
                return FunctionalApplicationContext<OUT_T, MAPPER>  (fMappingEngine_.Map (inherited (*this), do2Each), fMappingEngine_);
            }
            template    <typename T, typename MAPPER>
            template    <typename OUT_T>
            inline  OUT_T                                           FunctionalApplicationContext<T, MAPPER>::Reduce (const function<OUT_T(T, OUT_T)>& do2Each, OUT_T memo)
            {
                return  fMappingEngine_.Reduce (inherited (*this), do2Each, memo);
            }
            template    <typename T, typename MAPPER>
            inline  FunctionalApplicationContext<T, MAPPER>       FunctionalApplicationContext<T, MAPPER>::Filter (const function<bool(T)>& includeTest)
            {
                return FunctionalApplicationContext<T>  (fMappingEngine_.Filter (inherited (*this), includeTest), fMappingEngine_);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_FunctionalApplication_inl_ */
