/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_

#include    "../../StroikaPreComp.h"

#include    "../Collection.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
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
                 *  \brief Collection_stdforward_list<T, TRAITS> is an std::forward_list (singly linked list)-based concrete implementation of the Collection<T, TRAITS> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = Collection_DefaultTraits<T>>
                class  Collection_stdforward_list : public Collection<T, TRAITS> {
                private:
                    typedef     Collection<T, TRAITS>  inherited;

                public:
                    Collection_stdforward_list ();
                    Collection_stdforward_list (const T* start, const T* end);
                    Collection_stdforward_list (const Collection<T, TRAITS>& collection);
                    Collection_stdforward_list (const Collection_stdforward_list<T, TRAITS>& collection);

                    nonvirtual  Collection_stdforward_list<T, TRAITS>&  operator= (const Collection_stdforward_list<T, TRAITS>& collection);

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
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
#include    "Collection_stdforward_list.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_ */
