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
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief Collection_stdforward_list<T> is an std::forward_list (singly linked list)-based concrete implementation of the Collection<T> container pattern.
     *
     * \note Performance Notes:
     *      o   GetLength () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Collection_stdforward_list : public Collection<T> {
    private:
        using inherited = Collection<T>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \brief STDFORWARDLIST is std::forward_list<> that can be used inside Collection_stdforward_list
         */
        using STDFORWARDLIST = forward_list<value_type, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<value_type, sizeof (value_type) <= 1024>>;

    public:
        /**
         */
        Collection_stdforward_list ();
        Collection_stdforward_list (const T* start, const T* end);
        Collection_stdforward_list (const Collection<T>& src);
        Collection_stdforward_list (const Collection_stdforward_list& src) noexcept = default;
        Collection_stdforward_list (Collection_stdforward_list&& src) noexcept      = default;

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
