/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Deque_h_
#define _Stroika_Foundation_Containers_Deque_h_ 1

#include "../StroikaPreComp.h"

#include "Queue.h"

/*
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *
 *  TODO:
 *
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   Add CTOR(Iterator<T>) after next release....
 *
 *      @todo   Consider  using the words front/back and push_front/pop_back etc - like with STL
 *
 */

namespace Stroika::Foundation {
    namespace Containers {

        using Configuration::ArgByValueType;
        using Traversal::Iterable;
        using Traversal::Iterator;

        /**
         * Description:
         *
         *      A Deque is a Queue that allows additions and removals at either end.
         *
         *      Deques - like Queues - iterate from Head to Tail.
         *
         * Notes:
         *
         *  <<NOTE - FUTURE WORK - AND DONT DOCUMENT DEFAULTIMPL HERE>>> - SEE FACTORY CODE
         *      We currently default to the circular array implementation, as it is
         *  fastest under most circumstances. One drawback to it is that it has
         *  unpredictable costs for an Add operation. DoubleLinkList is usually
         *  slower, but has very predictable costs.
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *
         *  \note Note About Iterators
         *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
         *
         *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
         *          the iterators are automatically updated internally to behave sensibly.
         *
         */
        template <typename T>
        class Deque : public Queue<T> {
        private:
            using inherited = Queue<T>;

        public:
            /**
             *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
             */
            using ArchetypeContainerType = Deque<T>;

        protected:
            class _IRep;

        protected:
            using _DequeRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

        public:
            /**
             *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add ()
             *
             *  \todo https://stroika.atlassian.net/browse/STK-651 - Experimental feature which might be used as a concept check on various templates
             */
            template <typename POTENTIALLY_ADDABLE_T>
            static constexpr bool IsAddable = is_convertible_v<POTENTIALLY_ADDABLE_T, T>;

        public:
            /**
             */
            Deque ();
            Deque (const Deque& src) noexcept = default;
            Deque (Deque&& src) noexcept      = default;
            Deque (const initializer_list<T>& src);
            template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Deque<T>*>>* = nullptr>
            Deque (const CONTAINER_OF_ADDABLE& src);
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            Deque (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

        protected:
            /**
             */
            explicit Deque (const _DequeRepSharedPtr& src) noexcept;
            explicit Deque (_DequeRepSharedPtr&& src) noexcept;

        public:
            /**
             */
            nonvirtual Deque& operator= (const Deque& rhs) = default;
            nonvirtual Deque& operator= (Deque&& rhs) = default;

        public:
            /**
             */
            nonvirtual void AddHead (ArgByValueType<T> item);

        public:
            /**
             */
            nonvirtual T RemoveTail ();

        public:
            /**
             */
            nonvirtual T Tail () const;

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
         *  \brief  Implementation detail for Deque<T> implementors.
         *
         *  Protected abstract interface to support concrete implementations of
         *  the Deque<T> container API.
         */
        template <typename T>
        class Deque<T>::_IRep : public Queue<T>::_IRep {
        private:
            using inherited = typename Queue<T>::_IRep;

        public:
            virtual void AddHead (ArgByValueType<T> item) = 0;
            virtual T    RemoveTail ()                    = 0;
            virtual T    Tail () const                    = 0;
        };
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Deque.inl"

#endif /*_Stroika_Foundation_Containers_Deque_h_ */
