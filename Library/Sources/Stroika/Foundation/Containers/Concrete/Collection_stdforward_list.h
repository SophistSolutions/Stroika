/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <forward_list>

#include "../Collection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief Collection_stdforward_list<T> is an std::forward_list (singly linked list)-based concrete implementation of the Collection<T> container pattern.
     *
     * \note Performance Notes:
     *      o   size () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Collection_stdforward_list : public Collection<T> {
    private:
        using inherited = Collection<T>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \brief STDFORWARDLIST is std::forward_list<> that can be used inside Collection_stdforward_list
         */
        using STDFORWARDLIST = forward_list<value_type, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<value_type, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  \see docs on Collection<T> constructor
         */
        Collection_stdforward_list ();
        Collection_stdforward_list (Collection_stdforward_list&& src) noexcept      = default;
        Collection_stdforward_list (const Collection_stdforward_list& src) noexcept = default;
        Collection_stdforward_list (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Collection_stdforward_list<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        Collection_stdforward_list (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Collection_stdforward_list (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Collection_stdforward_list& operator= (Collection_stdforward_list&& rhs) noexcept = default;
        nonvirtual Collection_stdforward_list& operator= (const Collection_stdforward_list& rhs) = default;

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
#include "Collection_stdforward_list.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_ */
