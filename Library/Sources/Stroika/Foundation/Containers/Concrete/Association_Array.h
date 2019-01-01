/*
* Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
*/
#include "../../StroikaPreComp.h"

#include "../Association.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Association_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Association_Array_h_

/**
*  \file
*
*  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
***VERY ROUGH UNUSABLE DRAFT*
*  TODO:
*      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
*              THEN - MAYBE - try todo better, but at least do this as starter
*/

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an Array-based concrete implementation of the Association<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::AssociationTraitsType> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_Array : public Association<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Association<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        Association_Array ();
        Association_Array (const Association_Array& src) = default;
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_base_of_v<Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        Association_Array (CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_KEY_T>
        Association_Array (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

    public:
        nonvirtual Association_Array& operator= (const Association_Array& rhs) = default;

    public:
        /**
         *  \brief  Reduce the space used to store the Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete Association, calling this may save memory.
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
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;
    };
}

/*
********************************************************************************
******************************* Implementation Details *************************
********************************************************************************
*/

#include "Association_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_Array_h_ */
