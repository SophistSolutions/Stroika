/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Set_Factory_h_

#include    "../../StroikaPreComp.h"

#include    <atomic>



/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkSet_Fastest,
 *              mkSet_Smallest, mkSetWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Rethink use of TRAITS as argument to factory. Probably can only do the SFINAE stuff
 *              when TRAITS EQUALS the default.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename TRAITS>
            class   Set;


            namespace   Concrete {


                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a Set<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling Set_Factory<T,TRAITS>::Register (), or
                 *  replace it statically by template-specailizing Set_Factory<T,TRAITS>::mk () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 */
                template    <typename T, typename TRAITS>
                class   Set_Factory {
                private:
                    static  atomic<Set<T, TRAITS> (*) ()>   sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Set<T,TRAITS> CTOR does so automatically.
                     */
                    static  Set<T, TRAITS>  mk ();

                public:
                    /**
                     *  Register a replacement creator/factory for the given Set<T,TRAITS>. Note this is a global change.
                     */
                    static  void    Register (Set<T, TRAITS> (*factory) () = nullptr);

                private:
                    static  Set<T, TRAITS>  Default_ ();

                private:
                    template    <typename CHECK_T>
                    static  Set<T, TRAITS>  Default_SFINAE_ (typename enable_if <Common::Has_Operator_LessThan<CHECK_T>::value>::type* = 0);
                    template    <typename CHECK_T>
                    static  Set<T, TRAITS>  Default_SFINAE_ (typename enable_if < !Common::Has_Operator_LessThan<CHECK_T>::value >::type* = 0);
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
#include    "Set_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Set_Factory_h_ */
