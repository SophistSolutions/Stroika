/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_h_

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
            class   SortedCollection;


            namespace   Concrete {


                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a SortedCollection<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling SortedCollection_Factory<T,TRAITS>::Register (), or
                 *  replace it statically by template-specailizing SortedCollection_Factory<T,TRAITS>::mk () - though the later is trickier.
                 *
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 */
                template    <typename T, typename TRAITS>
                class   SortedCollection_Factory {
                private:
                    static  atomic<SortedCollection<T, TRAITS> (*) ()> sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Collection<T> CTOR does so automatically.
                     */
                    static  SortedCollection<T, TRAITS>  mk ();

                public:
                    /**
                     *  Register a replacement creator/factory for the given Collection<T>. Note this is a global change.
                     */
                    static  void    Register (SortedCollection<T, TRAITS> (*factory) () = nullptr);

                private:
                    static  SortedCollection<T, TRAITS>  Default_ ();
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
#include    "SortedCollection_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_h_ */
