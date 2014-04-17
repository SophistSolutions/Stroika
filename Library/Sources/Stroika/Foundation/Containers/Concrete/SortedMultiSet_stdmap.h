/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_   1

#include    "../../StroikaPreComp.h"

#include    "../SortedMultiSet.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Get use of Private::IteratorImplHelper_ working
 *
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals ()
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = SortedMultiSet_DefaultTraits<T>>
                class   SortedMultiSet_stdmap : public SortedMultiSet<T, TRAITS> {
                private:
                    using   inherited   =   SortedMultiSet<T, TRAITS>;

                public:
                    /**
                     */
                    SortedMultiSet_stdmap ();
                    SortedMultiSet_stdmap (const SortedMultiSet_stdmap<T, TRAITS>& src);
                    SortedMultiSet_stdmap (const std::initializer_list<T>& src);
                    SortedMultiSet_stdmap (const std::initializer_list<MultiSetEntry<T>>& src);
                    template <typename CONTAINER_OF_T>
                    explicit SortedMultiSet_stdmap (const CONTAINER_OF_T& src);
                    SortedMultiSet_stdmap (const T* start, const T* end);
                    SortedMultiSet_stdmap (const MultiSetEntry<T>* start, const MultiSetEntry<T>* end);

                public:
                    /**
                    */
                    nonvirtual  SortedMultiSet_stdmap<T, TRAITS>& operator= (const SortedMultiSet_stdmap<T, TRAITS>& src);

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class Rep_;

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
#include    "SortedMultiSet_stdmap.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_*/

