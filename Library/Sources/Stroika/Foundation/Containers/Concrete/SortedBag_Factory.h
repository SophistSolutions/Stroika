/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedBag_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedBag_Factory_h_

#include    "../../StroikaPreComp.h"

#include    <atomic>



/**
 *  \file
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename TRAITS>
            class   SortedBag;


            namespace   Concrete {


                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a SortedBag<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling SortedBag_Factory<T,TRAITS>::Register (), or
                 *  replace it statically by template-specailizing SortedBag_Factory<T,TRAITS>::mk () - though the later is trickier.
                 *
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 */
                template    <typename T, typename TRAITS>
                class   SortedBag_Factory {
                private:
                    static  atomic<SortedBag<T, TRAITS> (*) ()> sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Bag<T,TRAITS> CTOR does so automatically.
                     */
                    static  SortedBag<T, TRAITS>  mk ();

                public:
                    /**
                     *  Register a replacement creator/factory for the given Bag<T,TRAITS>. Note this is a global change.
                     */
                    static  void    Register (SortedBag<T, TRAITS> (*factory) () = nullptr);

                private:
                    static  SortedBag<T, TRAITS>  Default_ ();
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
#include    "SortedBag_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedBag_Factory_h_ */
