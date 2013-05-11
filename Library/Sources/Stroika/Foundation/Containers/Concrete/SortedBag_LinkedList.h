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
 *  TODO:
 *
 *      @todo   DO SORTING. Just be careful in ADD method. ITs that simple! But first, fix REGRESSION
 *              test so it actually DETECTS that this ist't working, and then fix it - so we can test
 *              fix worked!
 *
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief SortedBag_LinkedList<T> is an LinkedList-based concrete implementation of the SortedBag<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                class  SortedBag_LinkedList : public SortedBag<T> {
                private:
                    typedef     SortedBag<T>  inherited;

                public:
                    SortedBag_LinkedList ();
                    SortedBag_LinkedList (const T* start, const T* end);
                    SortedBag_LinkedList (const SortedBag<T>& sb);
                    SortedBag_LinkedList (const SortedBag_LinkedList<T>& sb);

                    nonvirtual  SortedBag_LinkedList<T>&  operator= (const SortedBag_LinkedList<T>& rhs);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
                    class   IteratorRep_;
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
