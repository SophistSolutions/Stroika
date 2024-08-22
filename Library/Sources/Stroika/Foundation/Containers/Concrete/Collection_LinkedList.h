/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_LinkedList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Collection.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief Collection_LinkedList<T> is an LinkedList-based concrete implementation of the Collection<T> container pattern.
     *
     * \note Performance Notes:
     *      o   size () is O(N), but empty () is O(1)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Collection_LinkedList : public Collection<T> {
    private:
        using inherited = Collection<T>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \see docs on Collection<T> constructor
         */
        Collection_LinkedList ();
        Collection_LinkedList (Collection_LinkedList&& src) noexcept      = default;
        Collection_LinkedList (const Collection_LinkedList& src) noexcept = default;
        Collection_LinkedList (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Collection_LinkedList<T>>)
        Collection_LinkedList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Collection_LinkedList{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Collection_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Collection_LinkedList& operator= (Collection_LinkedList&& rhs) noexcept = default;
        nonvirtual Collection_LinkedList& operator= (const Collection_LinkedList& rhs)     = default;

    private:
        class IImplRep_;
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
#include "Collection_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Collection_LinkedList_h_ */
