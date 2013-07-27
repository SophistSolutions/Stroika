/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_stdforward_list_h_
#define _Stroika_Foundation_Containers_Concrete_Bag_stdforward_list_h_

#include    "../../StroikaPreComp.h"

#include    "../Bag.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief Bag_stdforward_list<T, TRAITS> is an std::forward_list (singly linked list)-based concrete implementation of the Bag<T, TRAITS> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = Bag_DefaultTraits<T>>
                class  Bag_stdforward_list : public Bag<T, TRAITS> {
                private:
                    typedef     Bag<T, TRAITS>  inherited;

                public:
                    Bag_stdforward_list ();
                    Bag_stdforward_list (const T* start, const T* end);
                    Bag_stdforward_list (const Bag<T, TRAITS>& bag);
                    Bag_stdforward_list (const Bag_stdforward_list<T, TRAITS>& bag);

                    nonvirtual  Bag_stdforward_list<T, TRAITS>&  operator= (const Bag_stdforward_list<T, TRAITS>& bag);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
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
#include    "Bag_stdforward_list.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Bag_stdforward_list_h_ */
