/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Factory_h_

#include    "../../StroikaPreComp.h"

#include    <atomic>



/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkSequence_Fasted,
 *              mkSequence_Smallest, mkSequenceWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename TRAITS>
            class   Sequence;


            namespace   Concrete {


                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a Sequence<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling Sequence_Factory<T,TRAITS>::Register (), or
                 *  replace it statically by template-specailizing Sequence_Factory<T,TRAITS>::mk () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 */
                template    <typename T, typename TRAITS>
                class   Sequence_Factory {
                private:
                    static  atomic<Sequence<T, TRAITS> (*) ()>   sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Sequence<T,TRAITS> CTOR does so automatically.
                     */
                    static  Sequence<T, TRAITS>  mk ();

                public:
                    /**
                     *  Register a replacement creator/factory for the given Sequence<T,TRAITS>. Note this is a global change.
                     */
                    static  void    Register (Sequence<T, TRAITS> (*factory) () = nullptr);

                private:
                    static  Sequence<T, TRAITS>  Default_ ();
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
#include    "Sequence_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Sequence_Factory_h_ */
