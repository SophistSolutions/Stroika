/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Bag_Array_h_

#include    "../../StroikaPreComp.h"

#include    "../Bag.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Change the default traits to contain info about realloc / array size stuff (maybe initial size?)
 *
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 *
 *      @todo   Fix Bag_Array<T, TRAITS>::Rep_::Remove() to use BackwardIterator
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 * \brief   Bag_Array<T> is an Array-based concrete implementation of the Bag<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = Bag_DefaultTraits<T>>
                class   Bag_Array : public Bag<T, TRAITS> {
                private:
                    typedef     Bag<T, TRAITS>  inherited;

                public:
                    Bag_Array ();
                    Bag_Array (const Bag<T, TRAITS>& bag);
                    Bag_Array (const Bag_Array<T, TRAITS>& bag);
                    Bag_Array (const T* start, const T* end);

                public:
                    nonvirtual  Bag_Array<T, TRAITS>& operator= (const Bag_Array<T, TRAITS>& bag);

                public:
                    /**
                     *  \brief  Reduce the space used to store the Bag<T> contents.
                     *
                     *  This has no semantics, no observable behavior. But depending on the representation of
                     *  the concrete bag, calling this may save memory.
                     */
                    nonvirtual  void    Compact ();

                public:
                    /*
                     * This optional API allows pre-reserving space as an optimizaiton.
                     */
                    nonvirtual  size_t  GetCapacity () const;
                    nonvirtual  void    SetCapacity (size_t slotsAlloced);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;

                private:
                    nonvirtual  const Rep_&  GetRep_ () const;
                    nonvirtual  Rep_&        GetRep_ ();
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

#include    "Bag_Array.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Bag_Array_h_ */
