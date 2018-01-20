/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Association.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Association_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Association_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS> is an LinkedList-based concrete implementation of the Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = DefaultTraits::Association<KEY_TYPE, VALUE_TYPE>>
                class Association_LinkedList : public Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType> {
                private:
                    using inherited = Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>;

                public:
                    Association_LinkedList ();
                    Association_LinkedList (const Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>& src);
                    template <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Association_LinkedList (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Association_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    nonvirtual Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) = default;

                private:
                    class Rep_;

                private:
                    nonvirtual void AssertRepValidType_ () const;
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

#include "Association_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_LinkedList_h_ */
