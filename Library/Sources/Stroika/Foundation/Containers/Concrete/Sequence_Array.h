/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Array_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Private/ArraySupport.h"
#include "Stroika/Foundation/Containers/Sequence.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_Array<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      o   push_back should perform well (typically constant time, but occasionally O(N))
     *      o   it is FAST to array index a Sequence_Array.
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Sequence_Array : public Private::ArrayBasedContainer<Sequence_Array<T>, Sequence<T>, false> {
    private:
        using inherited = Private::ArrayBasedContainer<Sequence_Array<T>, Sequence<T>, false>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \see docs on Sequence<> constructor
         */
        Sequence_Array ();
        Sequence_Array (Sequence_Array&&) noexcept      = default;
        Sequence_Array (const Sequence_Array&) noexcept = default;
        Sequence_Array (const initializer_list<value_type>& src);
        template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Sequence_Array<T>>)
        explicit Sequence_Array (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Sequence_Array{}
        {
            if constexpr (Common::IHasSizeMethod<ITERABLE_OF_ADDABLE>) {
                this->reserve (src.size ());
            }
            this->AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Sequence_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Sequence_Array& operator= (Sequence_Array&&) noexcept = default;
        nonvirtual Sequence_Array& operator= (const Sequence_Array&)     = default;

    private:
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;

    private:
        friend class Private::ArrayBasedContainer<Sequence_Array, Sequence<T>, false>;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Sequence_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_Array_h_ */
