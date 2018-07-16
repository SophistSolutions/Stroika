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
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                class Mapping_LinkedList : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
                private:
                    using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

                public:
                    /**
                     *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
                     */
                    Mapping_LinkedList ();
                    template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>* = nullptr>
                    explicit Mapping_LinkedList (const KEY_EQUALS_COMPARER& keyEqualsComparer);
                    Mapping_LinkedList (const Mapping_LinkedList& src) = default;
                    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>*>>* = nullptr>
                    explicit Mapping_LinkedList (const CONTAINER_OF_ADDABLE& src);
                    template <typename COPY_FROM_ITERATOR_KEYVALUE>
                    explicit Mapping_LinkedList (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

                public:
                    nonvirtual Mapping_LinkedList& operator= (const Mapping_LinkedList& rhs) = default;

                private:
                    class IImplRepBase_;
                    template <typename KEY_EQUALS_COMPARER>
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
