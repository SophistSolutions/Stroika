/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Tally_Factory_h_

#include    "../../StroikaPreComp.h"

#include    <atomic>


/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkTally_Fastest,
 *              mkTally_Smallest, mkTallyWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename TRAITS>
            class   Tally;


            namespace   Concrete {


                /**
                *  \brief   Singleton factory object - Used to create the default backend implementation of a Tally<> container
                *
                *  Note - you can override the underlying factory dynamically by calling Tally_Factory<T,TRAITS>::Register (), or
                *  replace it statically by template-specailizing Tally_Factory<T,TRAITS>::mk () - though the later is trickier.
                *
                *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                */
                template    <typename T, typename TRAITS>
                class   Tally_Factory {
                private:
                    static  atomic<Tally<T, TRAITS> (*) ()>   sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Tally<T,TRAITS> CTOR does so automatically.
                     */
                    static  Tally<T, TRAITS>  mk ();

                public:
                    /**
                     *  Register a replacement creator/factory for the given Tally<T,TRAITS>. Note this is a global change.
                     */
                    static  void    Register (Tally<T, TRAITS> (*factory) () = nullptr);

                private:
                    static  Tally<T, TRAITS>  Default_ ();
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
#include    "Tally_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Tally_Factory_h_ */
