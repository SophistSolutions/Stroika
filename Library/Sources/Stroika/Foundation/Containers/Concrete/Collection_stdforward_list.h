/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include <forward_list>

#include "Stroika/Foundation/Containers/Collection.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief Collection_stdforward_list<T> is an std::forward_list (singly linked list)-based concrete implementation of the Collection<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      o   size () is O(N), but empty () is O(1)
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
         *  \see docs on Collection<T> constructor
         */
        Collection_stdforward_list ();
        Collection_stdforward_list (Collection_stdforward_list&&) noexcept      = default;
        Collection_stdforward_list (const Collection_stdforward_list&) noexcept = default;
        Collection_stdforward_list (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Collection_stdforward_list<T>>)
        Collection_stdforward_list (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Collection_stdforward_list{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Collection_stdforward_list (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Collection_stdforward_list& operator= (Collection_stdforward_list&&) noexcept = default;
        nonvirtual Collection_stdforward_list& operator= (const Collection_stdforward_list&)     = default;

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
