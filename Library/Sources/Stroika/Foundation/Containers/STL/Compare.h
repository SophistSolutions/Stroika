/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_STL_Compare_h_
#define _Stroika_Foundation_Containers_STL_Compare_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/TypeHints.h"
#include    "../../Common/Compare.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace STL {


                /**
                 *  An STL-compatible variant of 'less' which leverages a stroika comparer. This can be used to use
                 *  Stroika comparares (almost directly) with std STL classes such as map<>, and set<>.
                 */
                template<typename T, typename STROIKA_COMPARER>
                struct less : public binary_function<T, T, bool> {
                    nonvirtual  bool operator() (Configuration::ArgByValueType<T> _Left, Configuration::ArgByValueType<T> _Right) const;
                };


                /**
                 *  Apply Stroika equals comparer to STL (iterators or containers) and return true iff equal.
                 */
                template <typename STROIKA_EQUALS_COMPARER, typename ITERATOR_OF_T>
                bool    Equals (ITERATOR_OF_T lhsStart, ITERATOR_OF_T lhsEnd, ITERATOR_OF_T rhsStart, ITERATOR_OF_T rhsEnd);
                template <typename STROIKA_EQUALS_COMPARER, typename CONTAINER_OF_T>
                bool    Equals (const CONTAINER_OF_T& lhs, const CONTAINER_OF_T& rhs);


            }
        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Compare.inl"

#endif  /*_Stroika_Foundation_Containers_STL_Compare_h_*/
