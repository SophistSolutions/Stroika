/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedBag_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedBag_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../SortedBag.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief SortedBag_LinkedList<T> is an LinkedList-based concrete implementation of the SortedBag<T> container pattern.
                 *
                 *  \note   \em Performance
                 *      SortedBag_LinkedList<T> is a compact, and reasonable implementation of bags, so long as the bag remains quite small
                 *      (empty or just a few entires). Thats really quite common!
                 *
                 *      However, add, contains tests, and removes are O(N) - so quite slow - when the bag grows.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 */
                template    <typename T, typename TRAITS = SortedBag_DefaultTraits<T>>
                class  SortedBag_LinkedList : public SortedBag<T, TRAITS> {
                private:
                    typedef     SortedBag<T, TRAITS>  inherited;

                public:
                    SortedBag_LinkedList ();
                    SortedBag_LinkedList (const T* start, const T* end);
                    SortedBag_LinkedList (const SortedBag<T, TRAITS>& sb);
                    SortedBag_LinkedList (const SortedBag_LinkedList<T, TRAITS>& sb);

                    nonvirtual  SortedBag_LinkedList<T, TRAITS>&  operator= (const SortedBag_LinkedList<T, TRAITS>& rhs);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
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
#include    "SortedBag_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedBag_LinkedList_h_ */
