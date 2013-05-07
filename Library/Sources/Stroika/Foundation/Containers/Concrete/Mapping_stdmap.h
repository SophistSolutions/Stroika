/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_

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
                 * \brief   Mapping_stdmap<Key, T> is an std::map-based concrete implementation of the Mapping<Key,T> container pattern.
                 */
                template    <typename Key, typename T>
                class   Mapping_stdmap : public Mapping<Key, T> {
                private:
                    typedef     Mapping<Key, T>  inherited;

                public:
                    Mapping_stdmap ();
                    Mapping_stdmap (const Mapping_stdmap<Key, T>& m);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Mapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_stdmap (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);


                public:
                    nonvirtual  Mapping_stdmap<Key, T>& operator= (const Mapping_stdmap<Key, T>& m);


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
#endif  /*_Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_ */


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Mapping_stdmap.inl"
