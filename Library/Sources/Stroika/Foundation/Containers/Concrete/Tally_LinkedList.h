/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals ()
 *
 *      @todo   Implementation currently stores list of T's rather than a list of
 *              TallyEntry<T>, and as a result computes tally's less efficiently
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchonizaiton support
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
                template    <typename T>
                class  Tally_LinkedList : public Tally<T> {
                private:
                    typedef Tally<T> inherited;

                public:
                    Tally_LinkedList ();
                    Tally_LinkedList (const Tally<T>& src);
                    Tally_LinkedList (const Tally_LinkedList<T>& src);
                    Tally_LinkedList (const T* items, size_t size);

                public:
                    nonvirtual  Tally_LinkedList<T>& operator= (const Tally_LinkedList<T>& src);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
                    class   IteratorRep_;
                };


            }
        }
    }
}



#include    "Tally_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Tally_LinkedList_h_ */

