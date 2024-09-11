/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Queue_h_
#define _Stroika_Foundation_Containers_Queue_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 *  TODO:
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
    using Traversal::IInputIterator;
    using Traversal::IIterableOf;
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
     *  \note Diagram:
     * 
     *            |   |     |   |     |   |     |   |
     *     FRONT=>| - |     | - |     | - |     | - |<= BACK
     *     (HEAD) | 0 |     | 1 |     | 2 |     | 3 | (TAIL)
     *            |   |     |   |     |   |     |   |
     *   items removed from this end                 new items added here
     *                       ^^dont access middle^^
     * 
     *  @see Deque<T> - which allow addition and removal at either end
     *  @see PriorityQueues<T, TRAITS> - which allow removal based on the priority
     *          assigned to an item.
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::Queue_Array<>
     *      o   @see Concrete::Queue_DoublyLinkedList<>
     *
     *  \em Factory:
     *      @see Queue_Factory<> to see default implementations.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   static_assert (totally_ordered<T> ==> totally_ordered<Queue<T>>);
     * 
     *      o   EqualsComparer provided as alias to SequentialEqualsComparer
     *          Two Queues are considered equal if they contain the same elements (by comparing them
     *          with EQUALS_COMPARER (which defaults to equal_to<T>)
     *          in exactly the same order (iteration).
     *      o   Since ordering in a Queue is well defined, we can use this ordering between elements to define
     *          the obvious sequential ordering three way comparison on queues (Iterable::SequentialThreeWayComparer)
     */
    template <typename T>
    class [[nodiscard]] Queue : public Iterable<T> {
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

    public:
        /**
         *  Construct a new Queue object. Overloads that take an argument container or start/end iterators, iterate from
         *  start to end, adding the items to the tail of the Queue, so the resulting Queue will be in the same order (when iterated or dequeued)
         *  as the order of the items its created from.
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        Queue ();
        Queue (Queue&&) noexcept      = default;
        Queue (const Queue&) noexcept = default;
        Queue (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Queue<T>>)
        explicit Queue (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Queue{}
        {
            AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Queue (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit Queue (shared_ptr<_IRep>&& rep) noexcept;
        explicit Queue (const shared_ptr<_IRep>& rep) noexcept;

    public:
        /**
         */
        nonvirtual Queue& operator= (Queue&&) noexcept = default;
        nonvirtual Queue& operator= (const Queue&)     = default;

    public:
        /**
         *  Add the given item to the end of the Q, so it will be removed last of all the items currently in the Q.
         *
         *  \note mutates container
         *
         *  \alias Enqueue, push_back
         */
        nonvirtual void AddTail (ArgByValueType<value_type> item);

    public:
        /**
         *  \brief Add the given item to the end of the Q, so it will be removed last of all the items currently in the Q (stlish alias)
         */
        nonvirtual void push_back (ArgByValueType<value_type> item);

    public:
        /**
         *  \req not empty ()
         *  @see HeadIf ()
         */
        nonvirtual value_type Head () const;

    public:
        /**
         *  \brief stlish alias for Head
         *  \req not empty ()
         */
        nonvirtual T front () const;

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
         *  \req not empty ()
         *  \note mutates container
         */
        nonvirtual value_type pop_back ();

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
         *  \req  IIterableOf<ITERABLE_OF_ADDABLE, T>  or IInputIterator<T>
         * 
         *  \note This works efficiently because a Queue<> iterates from head to tail, and that's the order in which you would want to
         *        add them to copy the Queue (unlike with Stack).
         *
         *  \note mutates container
         */
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        nonvirtual void AddAllToTail (ITERABLE_OF_ADDABLE&& s);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        nonvirtual void AddAllToTail (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

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
        template <typename ELEMENT_COMPARER = compare_three_way>
        using ThreeWayComparer = typename Iterable<T>::template SequentialThreeWayComparer<ELEMENT_COMPARER>;

    public:
        /**
         * simply indirect to @Queue<>::EqualsComparer
         */
        nonvirtual bool operator== (const Queue& rhs) const
            requires (equality_comparable<T>);

    public:
        /**
         * simply indirect to @Queue<>::ThreeWayComparer (only defined if T::operator<=> is defined)
         */
        nonvirtual auto operator<=> (const Queue& rhs) const
            requires (three_way_comparable<T>);

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

    public:
        virtual shared_ptr<_IRep>    CloneEmpty () const                       = 0;
        virtual void                 AddTail (ArgByValueType<value_type> item) = 0;
        virtual value_type           RemoveHead ()                             = 0;
        virtual optional<value_type> RemoveHeadIf ()                           = 0;
        virtual value_type           Head () const                             = 0;
        virtual optional<value_type> HeadIf () const                           = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Queue.inl"

#endif /*_Stroika_Foundation_Containers_Queue_h_ */
