/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Bag_Factory_h_

#include    "../../StroikaPreComp.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkBag_Fastest,
 *              mkBag_Smallest, mkBagWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Consider something like RegisterFactory_Bag below
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename TRAITS>
            class   Bag;


            namespace   Concrete {


                /**
                 *  \brief   Create the default backend implementation of a Bag<> container
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                Bag<T>  mkBag_Default ();


                // PROTO-IDEA - NOT IMPLEMENTED
#if 0
                template    <typename Key, typename T>
                void    RegisterFactory_Bag (Bag<Key, T> (*factory) () = nullptr);
#endif


            }
        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Bag_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Bag_Factory_h_ */
