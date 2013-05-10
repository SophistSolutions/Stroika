/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Set.h"


/**
 *  \file
 *
 *  TODO:
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 * \brief   Set_LinkedList<T> is an LinkedList-based concrete implementation of the Set<T> container pattern.
                 */
                template    <typename T>
                class   Set_LinkedList : public Set<T> {
                private:
                    typedef     Set<T>  inherited;

                public:
                    Set_LinkedList ();
                    Set_LinkedList (const Set_LinkedList<T>& m);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Set_LinkedList (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Set_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);


                public:
                    nonvirtual  Set_LinkedList<T>& operator= (const Set_LinkedList<T>& m);


                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
                    class   IteratorRep_;

                private:
                    nonvirtual  const Rep_&  GetRep_ () const;
                    nonvirtual  Rep_&        GetRep_ ();
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

#include    "Set_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_ */
