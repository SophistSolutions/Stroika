/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Mapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_

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
                 *  \brief   Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an LinkedList-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS = DefaultTraits::Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>>
                class Mapping_LinkedList : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> {
                private:
                    using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType>;

                public:
                    Mapping_LinkedList ();
                    Mapping_LinkedList (const Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>& src);
                    template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value && !std::is_convertible<const CONTAINER_OF_PAIR_KEY_T*, const Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>*>::value>::type>
                    explicit Mapping_LinkedList (const CONTAINER_OF_PAIR_KEY_T& src);
                    template <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    nonvirtual Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>& operator= (const Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>& rhs) = default;

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

#include "Mapping_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_ */
