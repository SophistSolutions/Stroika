/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Queue_h_
#define _Stroika_Foundation_Containers_Queue_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
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
 *      @todo   Consider adding 'max-size' feature to Q. With this set, attempt to add item past
 *              max size would throw. This can be handy with stuff like blocking q? and probably
 *              other cases as well (when you don't want q to grow indefinitely).
 *
 *      @todo   Select carefully thoguht through principle descriptive documemtatnion (probably: HEAD/TAIL)
 *              and use that docuemntation PRINCIPLALLY THORUHGOUT THE API (and class docs).
 *
 *      @todo   trial balloon - EnqueueAll(CONTAINER), and Enqueue (STARTIT, ENDIT), enques
 *              items in order from start to end, so that when iterating over the resulting queue, you
 *              encounter the items in the order you added them. That makes EnqueueAll(CONTINER) as simple
 *              as EnqueueAll(C.begin(),C.end()).
 *
 *      @todo   Embellish test cases (regression tests), and fix/make sure copying works.
 *
 *      @todo   Review
 *              http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Queue.hh
 *              for any important lackings
 *
 *      @todo   Draw a picture of q Q of people waiting in line (in docs below)
 *
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  \brief A Queue is a first-in-first-out (FIFO) data structure, where elements are arranged
     *         in well-ordered fashion, from HEAD to TAIL.
     *
     *  The HEAD if the Queue is where elements are removed. The TAIL of the queue is where
     *  elements enter the queue.
     *
     *  Queues always iterate from Head (aka front) to Tail: the same order as removals
     *  would encounter items, dequeing them.
     *
     *  @see Sedgewick, 30-31.(@todo CHECK REFERNECE)
     *
     *  Related classes include Deques, which allow addition and removal at
     *  either end, and PriorityQueues, which allow removal based on the priority
     *  assigned to an item.
     *
     *  @see Deque<T> - which allow addition and removal at either end
     *  @see PriorityQueues<T, TRAITS> - which allow removal based on the priority
     *          assigned to an item.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note Move constructor/assignment
     *      This maps to copy due to COW - see description of Iterable<T> for details.
     *
     *  \note Note About Iterators
     *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
     *
     *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
     *          the iterators are automatically updated internally to behave sensibly.
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison equality (==, !=) support (iff <T> parameter has default equal_to<T> implementation)
     *      o   EqualsComparer provided as alias to SequentialEqualsComparer
     *          Two Queues are considered equal if they contain the same elements (by comparing them
     *          with EQUALS_COMPARER (which defaults to equal_to<T>)
     *          in exactly the same order (iteration).
     *      o   Since ordering in a Queue is well defined, we can use this ordering between elements to define
     *          the obvious sequential ordering three way comparison on queues (Iterable::SequentialThreeWayComparer)
     */
    template <typename T>
    class Queue : public Iterable<T> {
    private:
        using inherited = Iterable<T>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Queue<T>;

    public:
        /**
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

    protected:
        class _IRep;

    protected:
        using _IRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (Enqueue)
         *
         *  \todo https://stroika.atlassian.net/browse/STK-651 - Experimental feature which might be used as a concept check on various templates
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable = is_convertible_v<POTENTIALLY_ADDABLE_T, value_type>;

    public:
        /**
         *  Construct a new Queue object. Overloads that take an argument container or start/end iterators, iterate from
         *  start to end, adding the items to the tail of the Queue, so the resulting Queue will be in the same order (when iterated or dequeued)
         *  as the order of the items its created from.
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         *
         *  \todo   @todo https://stroika.atlassian.net/browse/STK-744 - rethink details of Stroika Container constructors
         */
        Queue ();
        Queue (Queue&& src) noexcept      = default;
        Queue (const Queue& src) noexcept = default;
        Queue (const initializer_list<value_type>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Queue<T>, decay_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        explicit Queue (CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        Queue (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

    protected:
        explicit Queue (_IRepSharedPtr&& rep) noexcept;
        explicit Queue (const _IRepSharedPtr& rep) noexcept;

    public:
        /**
         */
        nonvirtual Queue& operator= (Queue&& rhs) = default;
        nonvirtual Queue& operator= (const Queue& rhs) = default;

    public:
        /**
         *  Add the given item to the end of the Q, so it will be removed last of all the items currently in the Q.
         *
         *  \note mutates container
         */
        nonvirtual void AddTail (ArgByValueType<value_type> item);

    public:
        /**
         *  \req not empty ()
         *  @see HeadIf ()
         */
        nonvirtual value_type Head () const;

    public:
        /**
         */
        nonvirtual optional<value_type> HeadIf () const;

    public:
        /**
         *  \req not empty ()
         *  \note mutates container
         */
        nonvirtual value_type RemoveHead ();

    public:
        /**
         *  \note mutates container
         */
        nonvirtual optional<value_type> RemoveHeadIf ();

    public:
        /**
         *  \brief Alias for AddTail () - add item to the end of the Q (line).
         *
         *  \note mutates container
         */
        nonvirtual void Enqueue (ArgByValueType<value_type> item);

    public:
        /**
         *  \brief Alias for RemoveHead () - remove item from the head of the Q (line).
         *
         *  \note mutates container
         *  \req not empty ()
         */
        nonvirtual value_type Dequeue ();

    public:
        /**
         *  Items are appended to the tail of the Q in same order they are encountered iterating from start to end of the container (or start to end iterators given).
         *
         *  This also implies that ordering will be preserved in iterating over the Queue, or in Dequeing those elements.
         *
         *  \note mutates container
         */
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE>>* = nullptr>
        nonvirtual void AddAllToTail (CONTAINER_OF_ADDABLE&& s);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        nonvirtual void AddAllToTail (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

    public:
        /**
         *  \note mutates container
         */
        nonvirtual void RemoveAll ();

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         *
         *  \note mutates container
         */
        nonvirtual void clear ();

    public:
        /**
         * \brief Simply indirect to @Iterable<T>::SequentialEqualsComparer
         */
        template <typename T_EQUALS_COMPARER = equal_to<T>>
        using EqualsComparer = typename Iterable<T>::template SequentialEqualsComparer<T_EQUALS_COMPARER>;

    public:
        /**
         */
        template <typename ELEMENT_COMPARER = Common::ThreeWayComparer<T, T>>
        using ThreeWayComparer = typename Iterable<T>::template SequentialThreeWayComparer<ELEMENT_COMPARER>;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         * simply indirect to @Queue<>::EqualsComparer (only defined if equal_to<T> is defined)
         */
        nonvirtual bool operator== (const Queue& rhs) const;

    public:
        /**
         * simply indirect to @Queue<>::ThreeWayComparer (only defined if T::operator<=> is defined)
         */
        nonvirtual auto operator<=> (const Queue& rhs) const;
#endif

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
        nonvirtual void _AssertRepValidType () const noexcept;
    };

    /**
     *  \brief  Implementation detail for Queue<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the Queue<T> container API.
     */
    template <typename T>
    class Queue<T>::_IRep : public Iterable<T>::_IRep {
    private:
        using inherited = typename Iterable<T>::_IRep;

    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    protected:
        using _IRepSharedPtr = typename Queue<T>::_IRepSharedPtr;

    public:
        virtual _IRepSharedPtr       CloneEmpty () const                       = 0;
        virtual void                 AddTail (ArgByValueType<value_type> item) = 0;
        virtual value_type           RemoveHead ()                             = 0;
        virtual optional<value_type> RemoveHeadIf ()                           = 0;
        virtual value_type           Head () const                             = 0;
        virtual optional<value_type> HeadIf () const                           = 0;
    };

#if __cpp_impl_three_way_comparison < 201907
    /**
     */
    template <typename T>
    bool operator== (const Queue<T>& lhs, const Queue<T>& rhs);
    template <typename T>
    bool operator!= (const Queue<T>& lhs, const Queue<T>& rhs);
#endif

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Queue.inl"

#endif /*_Stroika_Foundation_Containers_Queue_h_ */
