/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
 *
 *
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   Consider adding 'max-size' feature to Q. With this set, attempt to add item past
 *              max size would throw. This can be handy with stuff like blocking q? and probably
 *              other cases as well (when you dont want q to grow indefinitely).
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

namespace Stroika::Foundation {
    namespace Containers {

        using Configuration::ArgByValueType;
        using Traversal::Iterable;
        using Traversal::Iterator;

        /**
         *  A Queue is a first-in-first-out (FIFO) data structure, where elements are arranged
         *  n well-ordered fashion, from HEAD to TAIL.
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
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *
         *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
         *
         *  \note Note About Iterators
         *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
         *
         *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
         *          the iterators are automatically updated internally to behave sensibly.
         *
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

        protected:
            class _IRep;

        protected:
            using _QueueRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

        public:
            /**
             *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (Enqueue)
             *
             *  \todo https://stroika.atlassian.net/browse/STK-651 - Experimental feature which might be used as a concept check on various templates
             */
            template <typename POTENTIALLY_ADDABLE_T>
            static constexpr bool IsAddable = is_convertible_v<POTENTIALLY_ADDABLE_T, T>;

        public:
            /**
             *  Construct a new Queue object. Overloads that take an argument container or start/end iterators, iterate from
             *  start to end, adding the items to the tail of the Queue, so the resulting Queue will be in the same order (when iterated or dequeued)
             *  as the order of the items its created from.
             */
            Queue ();
            Queue (const Queue& src) noexcept = default;
            Queue (Queue&& src) noexcept      = default;
            Queue (const initializer_list<T>& src);
            template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, T>::value and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Queue<T>*>>* = nullptr>
            Queue (const CONTAINER_OF_ADDABLE& src);
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            Queue (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

        protected:
            explicit Queue (const _QueueRepSharedPtr& rep) noexcept;
            explicit Queue (_QueueRepSharedPtr&& rep) noexcept;

#if qDebug
        public:
            ~Queue ();
#endif

        public:
            /**
             */
            nonvirtual Queue<T>& operator= (const Queue<T>& rhs) = default;
            nonvirtual Queue<T>& operator= (Queue<T>&& rhs) = default;

        public:
            /**
             *  Add the given item to the end of the Q, so it will be removed last of all the items currently in the Q.
             */
            nonvirtual void AddTail (ArgByValueType<T> item);

        public:
            /**
             *  \req not empty ()
             *  @see HeadIf ()
             */
            nonvirtual T Head () const;

        public:
            /**
             */
            nonvirtual optional<T> HeadIf () const;

        public:
            /**
             */
            nonvirtual T RemoveHead ();

        public:
            /**
             */
            nonvirtual optional<T> RemoveHeadIf ();

        public:
            /**
             *  \brief Alias for AddTail () - add item to the end of the Q (line).
             */
            nonvirtual void Enqueue (ArgByValueType<T> item);

        public:
            /**
             *  \brief Alias for RemoveHead () - remove item from the head of the Q (line).
             */
            nonvirtual T Dequeue ();

        public:
            /**
             *  Items are appended to the tail of the Q in same order they are encountered iterating from start to end of the container (or start to end iterators given).
             *
             *  This also implies that ordering will be preserved in iterating over the Queue, or in Dequeing those elements.
             */
            template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::has_beginend<CONTAINER_OF_ADDABLE>::value>* = nullptr>
            nonvirtual void AddAllToTail (const CONTAINER_OF_ADDABLE& s);
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            nonvirtual void AddAllToTail (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

        public:
            /**
             */
            nonvirtual void RemoveAll ();

        public:
            /**
             * \brief STL-ish alias for RemoveAll ().
             */
            nonvirtual void clear ();

        public:
            /**
             *  Two Queues are considered equal if they contain the same elements (by comparing them
             *  with EQUALS_COMPARER (which defaults to operator== (T,T))
             *  in exactly the same order (iteration).
             *
             *  If == is predefined, you can just call Equals() - but if its not, or if you wish
             *  to compare with an alternative comparer, just pass it as a template parameter.
             *
             *  Equals is commutative().
             *
             *  Computational Complexity: O(N)
             */
            template <typename EQUALS_COMPARER = equal_to<T>>
            nonvirtual bool Equals (const Queue& rhs, const EQUALS_COMPARER& equalsComparer = {}) const;

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

        using Traversal::IteratorOwnerID;

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
            using _QueueRepSharedPtr = typename Queue<T>::_QueueRepSharedPtr;

        public:
            virtual _QueueRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const = 0;
            virtual void               AddTail (ArgByValueType<T> item)                       = 0;
            virtual T                  RemoveHead ()                                          = 0;
            virtual optional<T>        RemoveHeadIf ()                                        = 0;
            virtual T                  Head () const                                          = 0;
            virtual optional<T>        HeadIf () const                                        = 0;
#if qDebug
            virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const = 0;
#endif
        };

        /**
         *      Syntactic sugar for Equals()
         *
         *  \note   This function uses std::equal_to<T>, which in turn uses operator==(T,T). To
         *          use a different comparer, call Equals() directly.
         */
        template <typename T>
        bool operator== (const Queue<T>& lhs, const Queue<T>& rhs);

        /**
         *      Syntactic sugar for not Equals()
         *
         *  \note   This function uses std::equal_to<T>, which in turn uses operator==(T,T). To
         *          use a different comparer, call Equals() directly.
         */
        template <typename T>
        bool operator!= (const Queue<T>& lhs, const Queue<T>& rhs);
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Queue.inl"

#endif /*_Stroika_Foundation_Containers_Queue_h_ */
