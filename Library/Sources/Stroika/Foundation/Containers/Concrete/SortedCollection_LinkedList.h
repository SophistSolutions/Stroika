/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../SortedCollection.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals()
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief SortedCollection_LinkedList<T> is an LinkedList-based concrete implementation of the SortedCollection<T> container pattern.
                 *
                 *  \note   \em Performance
                 *      SortedCollection_LinkedList<T> is a compact, and reasonable implementation of Collections, so long as the Collection remains quite small
                 *      (empty or just a few entires). Thats really quite common!
                 *
                 *      However, add, contains tests, and removes are O(N) - so quite slow - when the Collection grows.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 */
                template    <typename T, typename TRAITS = SortedCollection_DefaultTraits<T>>
                class  SortedCollection_LinkedList : public SortedCollection<T, TRAITS> {
                private:
                    using   inherited   =     SortedCollection<T, TRAITS>;

                public:
                    SortedCollection_LinkedList ();
                    SortedCollection_LinkedList (const T* start, const T* end);
                    SortedCollection_LinkedList (const SortedCollection<T, TRAITS>& sb);
                    SortedCollection_LinkedList (const SortedCollection_LinkedList<T, TRAITS>& sb);

                public:
                    nonvirtual  SortedCollection_LinkedList<T, TRAITS>&  operator= (const SortedCollection_LinkedList<T, TRAITS>& rhs) = default;

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
#include    "SortedCollection_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_ */
