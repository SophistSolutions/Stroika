/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_h_  1

#include    "../../StroikaPreComp.h"

#include    "../Tally.h"



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
                template    <typename T, typename TRAITS = Tally_DefaultTraits<T>>
                class  Tally_LinkedList : public Tally<T, TRAITS> {
                private:
                    using   inherited   =   Tally<T, TRAITS>;

                public:
                    Tally_LinkedList ();
                    Tally_LinkedList (const Tally<T, TRAITS>& src);
                    Tally_LinkedList (const Tally_LinkedList<T, TRAITS>& src);
                    Tally_LinkedList (const std::initializer_list<T>& s);
                    Tally_LinkedList (const std::initializer_list<TallyEntry<T>>& s);
                    Tally_LinkedList (const T* start, const T* end);

                public:
                    nonvirtual  Tally_LinkedList<T, TRAITS>& operator= (const Tally_LinkedList<T, TRAITS>& rhs);

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



#include    "Tally_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Tally_LinkedList_h_ */

