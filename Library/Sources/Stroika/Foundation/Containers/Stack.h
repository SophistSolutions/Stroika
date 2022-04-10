/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_h_
#define _Stroika_Foundation_Containers_Stack_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Configuration/Concepts.h"
#include "../Execution/Synchronized.h"
#include "../Traversal/Iterable.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 *  TODO:
 *      @todo   Improve performance of CTOR's using reverse-iterator (?)
 *
 *      @todo   Embellish test cases (regression tests), and fix/make sure copying works.
 *
 *      @todo   Review
 *              http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Stack.hh
 *              for any important lackings
 *
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Configuration::ExtractValueType_t;
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
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o Standard Stroika Comparison equality (==, !=) support (iff <T> parameter has default equal_to<T> implementation)
     *      o   EqualsComparer provided as alias to SequentialEqualsComparer
     *          Two Queues are considered equal if they contain the same elements (by comparing them
     *          with EQUALS_COMPARER (which defaults to equal_to<T>)
     *          in exactly the same order (iteration).
     *      o   Since ordering in a Queue is well defined, we can use this ordering between elements to define
     *          the obvious sequential ordering three way comparison on queues (Iterable::SequentialThreeWayComparer)
     */
    template <typename T>
    class Stack : public Iterable<T> {
    private:
        using inherited = Iterable<T>;

    protected:
        class _IRep;

    protected:
        using _IRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;

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
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (Push)
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = is_convertible_v<POTENTIALLY_ADDABLE_T, T>;

    public:
        /**
         *  @todo   MUST WORK OUT DETAILS OF SEMANTICS FOR ITERATOR ADD cuz naive interpreation of above
         *          rules would lead to having a copy reverse the stack (SEE FILE-TODO-NOTE)
         * 
         *  \note When copying an Iterable<>, the order of items is REVERSED before pushing into the stack.
         *
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         *
         *  \todo   @todo https://stroika.atlassian.net/browse/STK-744 - rethink details of Stroika Container constructors
         */
        Stack ();
        Stack (Stack&& src) noexcept      = default;
        Stack (const Stack& src) noexcept = default;
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Stack<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Stack (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Stack (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit Stack (_IRepSharedPtr&& src) noexcept;
        explicit Stack (const _IRepSharedPtr& src) noexcept;

    public:
        /**
         */
        nonvirtual Stack& operator= (Stack&& rhs) noexcept = default;
        nonvirtual Stack& operator= (const Stack& rhs) = default;

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
        template <typename ELEMENT_COMPARER = Common::ThreeWayComparer>
        using ThreeWayComparer = typename Iterable<T>::template SequentialThreeWayComparer<ELEMENT_COMPARER>;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         * simply indirect to @Stack<>::EqualsComparer
         */
        nonvirtual bool operator== (const Stack& rhs) const;

    public:
        /**
         * simply indirect to @Stack<>::operator (only defined if ???comparethreeway?<T> is defined)
         */
        nonvirtual auto operator<=> (const Stack& rhs) const;
#endif

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

    protected:
        using _IRepSharedPtr = typename Stack<T>::_IRepSharedPtr;

    public:
        virtual _IRepSharedPtr CloneEmpty () const                    = 0;
        virtual void           Push (ArgByValueType<value_type> item) = 0;
        virtual value_type     Pop ()                                 = 0;
        virtual value_type     Top () const                           = 0;
    };

#if __cpp_impl_three_way_comparison < 201907
    /**
     *  Basic comparison operator overloads with the obvious meaning, and simply indirect to @Bijection<>::EqualsComparer
     */
    template <typename T>
    bool operator== (const Stack<T>& lhs, const Stack<T>& rhs);
    template <typename T>
    bool operator!= (const Stack<T>& lhs, const Stack<T>& rhs);
#endif

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Stack.inl"

#endif /*_Stroika_Foundation_Containers_Stack_h_ */
