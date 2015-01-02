/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_SparseArray_h_
#define _Stroika_Foundation_Containers_Concrete_Set_SparseArray_h_

#include    "../../StroikaPreComp.h"

#include    "../Set.h"



/**
 *  \file
 *
 *  \version    NOT IMPLEMENTED
 *
 *  TODO:
 *      @todo   Set_SparseArray<T> (or Set_SparseLinkedList>??)
 *              Assumes you can do T a, b, c; if (a < b and c< d) etc.So good for T = int..
 *
 *              Then store data as linkledlist pair<lb, ub> and walk and see if in any of the ranges.
 *              Or better – a tree (set<>> with top- compare done by LHS.
 *
 *              Nice to add coalese feature on add remove. A good ½ day to impement properly.
 *
 *              Probably also AsRuns -> Iterable<Range<T,T>>
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 */
                template    <typename T, typename TRAITS = Set_DefaultTraits<T>>
                class   Set_SparseArray : public Set<T, typename TRAITS::SetTraitsType> {
                private:
                    using   inherited   =     Set<T, typename TRAITS::SetTraitsType>;

                public:
                    Set_SparseArray ();
                    Set_SparseArray (const Set_SparseArray<T, TRAITS>& src);
                    Set_SparseArray (const std::initializer_list<T>& src);
                    template    <typename CONTAINER_OF_T>
                    explicit Set_SparseArray (const CONTAINER_OF_T& src);
                    template    <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Set_SparseArray (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);


                public:
                    nonvirtual  Set_SparseArray<T, TRAITS>& operator= (const Set_SparseArray<T, TRAITS>& rhs);


                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                private:
                    nonvirtual  void    AssertRepValidType_ () const;
                };


            }
        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

//#include    "Set_SparseArray.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Set_SparseArray_h_ */
