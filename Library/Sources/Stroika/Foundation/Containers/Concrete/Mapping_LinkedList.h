/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Mapping.h"


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
                 * \brief   Mapping_LinkedList<Key, T> is an LinkedList-based concrete implementation of the Mapping<Key,T> container pattern.
                 */
                template    <typename Key, typename T>
                class   Mapping_LinkedList : public Mapping<Key, T> {
                private:
                    typedef     Mapping<Key, T>  inherited;

                public:
                    Mapping_LinkedList ();
                    Mapping_LinkedList (const Mapping_LinkedList<Key, T>& m);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Mapping_LinkedList (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);


                public:
                    nonvirtual  Mapping_LinkedList<Key, T>& operator= (const Mapping_LinkedList<Key, T>& m);


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

#include    "Mapping_LinkedList.inl"


#endif  /*_Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_ */
