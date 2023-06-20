/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Sequence.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_Array<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
     *
     * \note Performance Notes:
     *      o   Append/Prepend should perform well (typically constant time, but occasionally O(N))
     *      o   it is FAST to array index a Sequence_Array.
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Sequence_Array : public Sequence<T> {
    private:
        using inherited = Sequence<T>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \see docs on Sequence<> constructor
         */
        Sequence_Array ();
        Sequence_Array (Sequence_Array&& src) noexcept      = default;
        Sequence_Array (const Sequence_Array& src) noexcept = default;
        Sequence_Array (const initializer_list<value_type>& src);
        template <IIterable<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Sequence_Array<T>>)
        explicit Sequence_Array (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Sequence_Array{}
        {
            if constexpr (Configuration::IHasSizeMethod<ITERABLE_OF_ADDABLE>) {
                reserve (src.size ());
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
        nonvirtual Sequence_Array& operator= (Sequence_Array&& rhs) noexcept = default;
        nonvirtual Sequence_Array& operator= (const Sequence_Array& rhs)     = default;

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
         *  \note Note that this does not affect the semantics of the Sequence.
         * 
         *  \req slotsAllocated >= size ()
         */
        nonvirtual void reserve (size_t slotsAlloced);

    public:
        /**
         *  \brief  Reduce the space used to store the Sequence<T> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete sequence, calling this may save memory.
         */
        nonvirtual void shrink_to_fit ();

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

#include "Sequence_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_Array_h_ */
