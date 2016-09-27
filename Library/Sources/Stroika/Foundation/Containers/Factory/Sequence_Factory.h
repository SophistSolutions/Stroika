/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Factory_h_

#include    "../../StroikaPreComp.h"

#include    <atomic>



/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkSequence_Fastest,
 *              mkSequence_Smallest, mkSequenceWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T>
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
                template    <typename T>
                class   Sequence_Factory {
                private:
                    static  atomic<Sequence<T> (*) (ContainerUpdateIteratorSafety)>   sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Sequence<T,TRAITS> CTOR does so automatically.
                     */
                    static  Sequence<T>  mk (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);

                public:
                    /**
                     *  Register a replacement creator/factory for the given Sequence<T,TRAITS>. Note this is a global change.
                     */
                    static  void    Register (Sequence<T> (*factory) (ContainerUpdateIteratorSafety) = nullptr);

                private:
                    static  Sequence<T>  Default_ (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy);
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
