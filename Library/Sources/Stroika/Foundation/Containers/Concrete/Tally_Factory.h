/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Tally_Factory_h_

#include    "../../StroikaPreComp.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkTally_Fasted,
 *              mkTally_Smallest, mkTallyWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Consider something like RegisterFactory_Tally below
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename TRAITS>
            class   Tally;


            namespace   Concrete {


                /**
                 *  \brief   Create the default backend implementation of a Tally<> container
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS>
                Tally<T, TRAITS>  mkTally_Default ();


                // PROTO-IDEA - NOT IMPLEMENTED
#if 0
                template    <typename T>
                void    RegisterFactory_Tally (Tally<T> (*factory) () = nullptr);
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
#include    "Tally_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Tally_Factory_h_ */
