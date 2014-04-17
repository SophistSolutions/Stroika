/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_h_  1

#include    "../../StroikaPreComp.h"

#include    "../MultiSet.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals ()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = MultiSet_DefaultTraits<T>>
                class  MultiSet_LinkedList : public MultiSet<T, TRAITS> {
                private:
                    using   inherited   =   MultiSet<T, TRAITS>;

                public:
                    MultiSet_LinkedList ();
                    MultiSet_LinkedList (const MultiSet<T, TRAITS>& src);
                    MultiSet_LinkedList (const MultiSet_LinkedList<T, TRAITS>& src);
                    MultiSet_LinkedList (const initializer_list<T>& src);
                    MultiSet_LinkedList (const initializer_list<MultiSetEntry<T>>& src);
                    MultiSet_LinkedList (const T* start, const T* end);

                public:
                    nonvirtual  MultiSet_LinkedList<T, TRAITS>& operator= (const MultiSet_LinkedList<T, TRAITS>& rhs) = default;

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                private:
                    nonvirtual  void    AssertRepValidType_ () const;
                };


            }
        }
    }
}



#include    "MultiSet_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_h_ */

