/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_h_

#include    "../../StroikaPreComp.h"

#include    <atomic>



/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkSortedMapping_Fastest,
 *              mkSortedMapping_Smallest, mkSortedMappingWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            class   SortedMapping;


            namespace   Concrete {


                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a SortedMapping<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling SortedMapping_Factory<T,TRAITS>::Register (), or
                 *  replace it statically by template-specailizing SortedMapping_Factory<T,TRAITS>::mk () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   SortedMapping_Factory {
                private:
                    static  atomic<SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> (*) (ContainerUpdateIteratorSafety)>   sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the SortedMapping<T,TRAITS> CTOR does so automatically.
                     */
                    static  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>  mk (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);

                public:
                    /**
                     *  Register a replacement creator/factory for the given SortedMapping<T,TRAITS>. Note this is a global change.
                     */
                    static  void    Register (SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> (*factory) (ContainerUpdateIteratorSafety) = nullptr);

                private:
                    static  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>  Default_ (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy);
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
#include    "SortedMapping_Factory.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_h_ */
