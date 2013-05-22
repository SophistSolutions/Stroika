/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedTally_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedTally_Factory_h_

#include    "../../StroikaPreComp.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkSortedTally_Fastest,
 *              mkSortedTally_Smallest, mkSortedTallyWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Consider something like RegisterFactory_SortedTally below
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T>
            class   SortedTally;


            namespace   Concrete {


                /**
                 *  \brief   Create the default backend implementation of a SortedTally<> container
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                SortedTally<T>  mkSortedTally_Default ();


                // PROTO-IDEA - NOT IMPLEMENTED
#if 0
                template    <typename T>
                void    RegisterFactory_SortedTally (SortedTally<T> (*factory) () = nullptr);
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
#include    "SortedTally_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedTally_Factory_h_ */
