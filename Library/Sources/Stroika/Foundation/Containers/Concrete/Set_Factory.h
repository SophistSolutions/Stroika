/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Set_Factory_h_

#include    "../../StroikaPreComp.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkSet_Fasted,
 *              mkSet_Smallest, mkSetWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Consider something like RegisterFactory_Set below
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename TRAITS>
            class   Set;


            namespace   Concrete {


                /**
                 *  \brief   Create the default backend implementation of a Set<> container
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS>
                Set<T, TRAITS>  mkSet_Default ();


                // PROTO-IDEA - NOT IMPLEMENTED
#if 0
                template    <typename Key, typename T>
                void    RegisterFactory_Set (Set<Key, T> (*factory) () = nullptr);
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
#include    "Set_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Set_Factory_h_ */
