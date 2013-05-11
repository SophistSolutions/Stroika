/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_h_

#include    "../../StroikaPreComp.h"



/**
 *  \file
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T>
            class   SortedSet;


            namespace   Concrete {


                /**
                 *  \brief   Create the default backend implementation of a SortedSet<> container
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                SortedSet<T>  mkSortedSet_Default ();


                // PROTO-IDEA - NOT IMPLEMENTED
#if 0
                template    <typename Key, typename T>
                void    RegisterFactory_SortedSet (SortedSet<Key, T> (*factory) () = nullptr);
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
#include    "SortedSet_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedSet_Factory_h_ */
