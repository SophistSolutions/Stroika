/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_h_
#define _Stroika_Foundation_Containers_Stack_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <ranges>

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Embellish test cases (regression tests), and fix/make sure copying works.
 *
 *      @todo   Review
 *              http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Stack.hh
 *              for any important lackings
 *
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Traversal::IInputIterator;
    using Traversal::IIterableOf;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *      Standard LIFO (Last in first out) Stack. See Sedgewick, 30-31.
     *      
     *  \note   Iteration proceeds from the top (last in & first out) to the bottom of the stack (first in & last out). Top
     *          is the LAST IN (also first out).
     *
     *  *Design Note*:
     *      We considered NOT having Stack<T> inherit from Iterable<T>, but that made copying of
     *      a stack intrinsically more costly, as you had to copy, and then pop items to see them,
     *      and put them into a new stack. A special copy API (private to stack) would have limited
     *      the ease of interoperating the Stack<T> container with other sorts of containers.
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::Stack_LinkedList<>
     *
     *  \em Factory:
     *      @see Stack_Factory<> to see default implementations.
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   static_assert (equality_comparable<T> ==> equality_comparable<Stack<T>>);
     *      o   EqualsComparer provided as alias to SequentialEqualsComparer
     *          Two Queues are considered equal if they contain the same elements (by comparing them
     *          with EQUALS_COMPARER (which defaults to equal_to<T>)
     *          in exactly the same order (iteration).
     *      o   Since ordering in a Queue is well defined, we can use this ordering between elements to define
     *          the obvious sequential ordering three way comparison on queues (Iterable::SequentialThreeWayComparer)
     */
    template <typename T>
    class [[nodiscard]] Stack : public Iterable<T> {
    private:
        using inherited = Iterable<T>;

    protected:
        class _IRep;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Stack<T>;

    public:
        /**
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \note When copying an Iterable<> or range, the copy is done by repeatedly pushing
         *        the arguments in the reverse order they are encountered, thus preserving the 'iteration order'
         *        of argument and copied stack.
         *
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        Stack ();
        Stack (Stack&&) noexcept      = default;
        Stack (const Stack&) noexcept = default;
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        template <typename ITERABLE_OF_ADDABLE,
                  enable_if_t<IIterableOf<ITERABLE_OF_ADDABLE, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Stack<T>>>* = nullptr>
        explicit Stack (ITERABLE_OF_ADDABLE&& src);
#else
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        explicit Stack (ITERABLE_OF_ADDABLE&& src)
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Stack<T>>);
#endif
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Stack (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit Stack (shared_ptr<_IRep>&& src) noexcept;
        explicit Stack (const shared_ptr<_IRep>& src) noexcept;

    public:
        /**
         */
        nonvirtual Stack& operator= (Stack&&) noexcept = default;
        nonvirtual Stack& operator= (const Stack&)     = default;

    public:
        /**
         *  \note mutates container
         */
        nonvirtual void Push (ArgByValueType<value_type> item);

    public:
        /**
         *  \note mutates container
         */
        nonvirtual value_type Pop ();

    public:
        /**
         */
        nonvirtual value_type Top () const;

    public:
        /**
         *  \note mutates container
         */
        nonvirtual void RemoveAll ();

    public:
        /**
         * \brief simply indirect to @Iterable<T>::SequentialEqualsComparer
         *
         *  Two Stack are considered equal if they contain the same elements in the same order.
         *  That is - @Iterable<T>::SequenceEquals.
         */
        template <typename T_EQUALS_COMPARER = equal_to<T>>
        using EqualsComparer = typename Iterable<T>::template SequentialEqualsComparer<T_EQUALS_COMPARER>;

    public:
        /**
         */
        template <typename ELEMENT_COMPARER = compare_three_way>
        using ThreeWayComparer = typename Iterable<T>::template SequentialThreeWayComparer<ELEMENT_COMPARER>;

    public:
        /**
         * simply indirect to @Stack<>::EqualsComparer
         */
        nonvirtual bool operator== (const Stack& rhs) const
            requires (equality_comparable<T>);

    public:
        /**
         * simply indirect to @Stack<>::operator (only defined if ???comparethreeway?<T> is defined)
         */
        nonvirtual auto operator<=> (const Stack& rhs) const
            requires (three_way_comparable<T>);

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         *
         *  \note mutates container
         */
        nonvirtual void clear ();

    protected:
        /**
         */
        template <typename T2>
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<T2>;

    protected:
        /**
         */
        template <typename T2>
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

    protected:
        nonvirtual void _AssertRepValidType () const;
    };

    /**
     *  \brief  Implementation detail for Stack<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the Stack<T> container API.
     */
    template <typename T>
    class Stack<T>::_IRep : public Iterable<T>::_IRep {
    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    public:
        virtual shared_ptr<_IRep> CloneEmpty () const                    = 0;
        virtual void              Push (ArgByValueType<value_type> item) = 0;
        virtual value_type        Pop ()                                 = 0;
        virtual value_type        Top () const                           = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Stack.inl"

#endif /*_Stroika_Foundation_Containers_Stack_h_ */
