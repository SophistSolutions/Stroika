/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bijection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Bijection_Factory_h_

#include    "../../StroikaPreComp.h"

#include    <atomic>
#include    <type_traits>


/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkBijection_Fastest,
 *              mkBijection_Smallest, mkBijectionWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Rethink use of TRAITS as argument to factory. Probably can only do the SFINAE stuff
 *              when TRAITS EQUALS the default.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            class   Bijection;


            namespace   Concrete {


                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a Bijection<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling Bijection_Factory<T,TRAITS>::Register (), or
                 *  replace it statically by template-specailizing Bijection_Factory<T,TRAITS>::mk () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 */
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                class   Bijection_Factory {
                private:
                    static  atomic<Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> (*) (ContainerUpdateIteratorSafety)>   sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Bijection<T,TRAITS> CTOR does so automatically.
                     */
                    static  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>  mk (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);

                public:
                    /**
                     *  Register a replacement creator/factory for the given Bijection<DOMAIN_TYPE, RANGE_TYPE,TRAITS>. Note this is a global change.
                     */
                    static  void    Register (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> (*factory) (ContainerUpdateIteratorSafety) = nullptr);

                private:
                    static  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>  Default_ (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy);

                private:
                    static  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>  Default_SFINAE_ (...);
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
#include    "Bijection_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Bijection_Factory_h_ */
