/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Collection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     * \brief   Collection_Array<T> is an Array-based concrete implementation of the Collection<T> container pattern.
     *
     * \note Performance Notes:
     *      A good low overhead, fast implementation. Adds and removes by iterator from the middle of the collection are slow.
     *      And adds are generally fast, but occasionally (on realloc) very slow once.
     *
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Collection_Array : public Collection<T> {
    private:
        using inherited = Collection<T>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \see docs on Collection<T> constructor
         */
        Collection_Array ();
        Collection_Array (Collection_Array&& src) noexcept      = default;
        Collection_Array (const Collection_Array& src) noexcept = default;
        Collection_Array (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Collection_Array<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        Collection_Array (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Collection_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Collection_Array& operator= (Collection_Array&& rhs) noexcept = default;
        nonvirtual Collection_Array& operator= (const Collection_Array& rhs) = default;

    public:
        /*
         *  \brief Return the number of allocated vector/array elements.
         * 
         * This optional API allows pre-reserving space as an optimization.
         * 
         *  \note alias GetCapacity ();
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         * This optional API allows pre-reserving space as an optimization.
         * 
         *  \note Alias SetCapacity ();
         * 
         *  \note Note that this does not affect the semantics of the Collection.
         * 
         *  \req slotsAllocated >= size ()
         */
        nonvirtual void reserve (size_t slotsAlloced);

    public:
        /**
         *  \brief  Reduce the space used to store the Collection<T> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete collection, calling this may save memory.
         */
        nonvirtual void shrink_to_fit ();

    protected:
        using _IterableRepSharedPtr   = typename inherited::_IterableRepSharedPtr;
        using _CollectionRepSharedPtr = typename inherited::_IRepSharedPtr;

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

#include "Collection_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Collection_Array_h_ */
