/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Collection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     * \brief   Collection_Array<T> is an Array-based concrete implementation of the Collection<T> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Collection_Array : public Collection<T> {
    private:
        using inherited = Collection<T>;

    public:
        /**
         */
        Collection_Array ();
        Collection_Array (const Collection<T>& src);
        Collection_Array (const Collection_Array& src) noexcept = default;
        Collection_Array (Collection_Array&& src) noexcept      = default;
        Collection_Array (const T* start, const T* end);

    public:
        nonvirtual Collection_Array& operator= (const Collection_Array& rhs) = default;

    public:
        /**
         *  \brief  Reduce the space used to store the Collection<T> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete collection, calling this may save memory.
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

#include "Collection_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Collection_Array_h_ */
