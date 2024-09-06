/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_Array_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Private/ArraySupport.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     * \brief   Collection_Array<T> is an Array-based concrete implementation of the Collection<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      A good low overhead, fast implementation. Adds and removes by iterator from the middle of the collection are slow.
     *      And adds are generally fast, but occasionally (on realloc) very slow once.
     *
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Collection_Array : public Private::ArrayBasedContainer<Collection_Array<T>, Collection<T>, false> {
    private:
        using inherited = Private::ArrayBasedContainer<Collection_Array<T>, Collection<T>, false>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \see docs on Collection<T> constructor
         */
        Collection_Array ();
        Collection_Array (Collection_Array&&) noexcept      = default;
        Collection_Array (const Collection_Array&) noexcept = default;
        Collection_Array (const initializer_list<value_type>& src);

        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Collection_Array<T>>)
        Collection_Array (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Collection_Array{}
        {
            this->reserve (src.size ());
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Collection_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Collection_Array& operator= (Collection_Array&&) noexcept = default;
        nonvirtual Collection_Array& operator= (const Collection_Array&)     = default;

    private:
        using IImplRep_ = typename Collection<T>::_IRep;
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
