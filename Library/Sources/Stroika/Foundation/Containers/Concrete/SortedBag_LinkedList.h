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
 *      @todo   DO SORTING. Just be careful in ADD method. ITs that simple! But first, fix REGRESSION
 *              test so it actually DETECTS that this ist't working, and then fix it - so we can test
 *              fix worked!
 *
 *              o   SortedBag_LinkedList<T>::Rep_::Update () is BUGGY!!! - change could change sort order. Must do
 *                  remove/add (or at least make sure we are already in right place when we update)
 *
 *              o   OK - more seriious - even Add() doesnt work ;-) thats more serious! Must have this functional
 *                  even if not performant!
 *
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchonizaiton support
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
