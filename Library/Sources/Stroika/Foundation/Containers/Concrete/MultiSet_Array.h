/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_   1


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
                class   MultiSet_Array : public MultiSet<T, TRAITS> {
                private:
                    using   inherited   =   MultiSet<T, TRAITS>;

                public:
                    MultiSet_Array ();
                    MultiSet_Array (const MultiSet<T, TRAITS>& src);
                    MultiSet_Array (const std::initializer_list<T>& s);
                    MultiSet_Array (const std::initializer_list<MultiSetEntry<T>>& s);
                    MultiSet_Array (const MultiSet_Array<T, TRAITS>& src);
                    MultiSet_Array (const T* start, const T* end);
                    MultiSet_Array (const MultiSetEntry<T>* start, const MultiSetEntry<T>* end);

                public:
                    nonvirtual  MultiSet_Array<T, TRAITS>& operator= (const MultiSet_Array<T, TRAITS>& rhs);

                public:
                    nonvirtual  size_t  GetCapacity () const;
                    nonvirtual  void    SetCapacity (size_t slotsAlloced);

                public:
                    nonvirtual  void    Compact ();

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



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "MultiSet_Array.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_*/

