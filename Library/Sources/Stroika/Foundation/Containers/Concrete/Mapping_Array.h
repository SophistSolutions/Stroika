/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Mapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an Array-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                class Mapping_Array : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
                private:
                    using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

                public:
                    /**
                     *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
                     */
                    Mapping_Array ();
                    template <typename KEY_EQUALS_COMPARER, typename ENABLE_IF_IS_COMPARER = enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>>
                    explicit Mapping_Array (const KEY_EQUALS_COMPARER& keyEqualsComparer, ENABLE_IF_IS_COMPARER* = nullptr);
                    Mapping_Array (const Mapping_Array& src) = default;
                    template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = enable_if_t<Configuration::has_beginend<CONTAINER_OF_ADDABLE>::value && !std::is_convertible<const CONTAINER_OF_ADDABLE*, const Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>*>::value>>
                    Mapping_Array (const CONTAINER_OF_ADDABLE& src);
                    template <typename COPY_FROM_ITERATOR_KEYVALUE>
                    Mapping_Array (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

                public:
                    nonvirtual Mapping_Array& operator= (const Mapping_Array& rhs) = default;

                public:
                    /**
                     *  \brief  Reduce the space used to store the Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> contents.
                     *
                     *  This has no semantics, no observable behavior. But depending on the representation of
                     *  the concrete Mapping, calling this may save memory.
                     */
                    nonvirtual void Compact ();

                public:
                    /*
                     * This optional API allows pre-reserving space as an optimization.
                     */
                    nonvirtual size_t GetCapacity () const;
                    nonvirtual void   SetCapacity (size_t slotsAlloced);

                public:
                    /**
                     *  STL-ish alias for GetCapacity ();
                     */
                    nonvirtual size_t capacity () const;

                public:
                    /**
                     *  STL-ish alias for SetCapacity ();
                     */
                    nonvirtual void reserve (size_t slotsAlloced);

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

#include "Mapping_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_Array_h_ */
