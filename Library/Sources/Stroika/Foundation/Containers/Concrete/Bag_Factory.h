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
 *      @todo   FIX FOR RE-ENTRANCY!!!! - USE STD-ATOMICS FOR UPDATE / FETCH OF FACTORY POINTER
 *
 *      @todo   Extend this metaphor to have different kinds of factories, like mkBag_Fastest,
 *              mkBag_Smallest, mkBagWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *              MAYBE????
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename TRAITS>
            class   Bag;


            namespace   Concrete {


                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a Bag<> container
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 */
                template    <typename T, typename TRAITS>
                class   Bag_Factory {
                private:
                    static  Bag<T, TRAITS>       (*sFactory_) ();

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Bag<T,TRAITS> CTOR does so automatically.
                     */
                    static  Bag<T, TRAITS>  mk ();

                public:
                    /**
                     *  Register a replacement creator/factory for the given Bag<T,TRAITS>. Note this is a global change.
                     */
                    static  void    Register (Bag<T, TRAITS> (*factory) () = nullptr);

                private:
                    static  Bag<T, TRAITS>  Default_ ();
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
#include    "Bag_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Bag_Factory_h_ */
