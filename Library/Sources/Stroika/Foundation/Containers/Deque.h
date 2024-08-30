/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Deque_h_
#define _Stroika_Foundation_Containers_Deque_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Queue.h"

/*
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Traversal::IInputIterator;
    using Traversal::IIterableOf;
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
     *  <<NOTE - FUTURE WORK - AND DON'T DOCUMENT DEFAULTIMPL HERE>>> - SEE FACTORY CODE
     *      We currently default to the circular array implementation, as it is
     *  fastest under most circumstances. One drawback to it is that it has
     *  unpredictable costs for an Add operation. DoubleLinkList is usually
     *  slower, but has very predictable costs.
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::Deque_DoublyLinkedList<>
     *
     *  \em Factory:
     *      @see Deque_Factory<> to see default implementations.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      @see inherited from Queue<T>
     */
    template <typename T>
    class [[nodiscard]] Deque : public Queue<T> {
    private:
        using inherited = Queue<T>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Deque<T>;

    public:
        /**
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

    protected:
        class _IRep;

    public:
        /**
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        Deque ();
        Deque (Deque&&) noexcept      = default;
        Deque (const Deque&) noexcept = default;
        Deque (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        explicit Deque (ITERABLE_OF_ADDABLE&& src)
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Deque<T>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Deque{}
        {
            this->AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Deque (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        /**
         */
        explicit Deque (shared_ptr<_IRep>&& src) noexcept;
        explicit Deque (const shared_ptr<_IRep>& src) noexcept;

    public:
        /**
         */
        nonvirtual Deque& operator= (Deque&&) noexcept = default;
        nonvirtual Deque& operator= (const Deque&)     = default;

    public:
        /**
         *  \note mutates container
         */
        nonvirtual void AddHead (ArgByValueType<value_type> item);

    public:
        /**
         *  \brief alias for AddHead
         */
        nonvirtual void push_front (ArgByValueType<value_type> item);

    public:
        /**
         *  \note mutates container
         */
        nonvirtual value_type RemoveTail ();

    public:
        /**
         *  \brief alias for AddHead
         */
        nonvirtual T pop_back ();

    public:
        /**
         */
        nonvirtual value_type Tail () const;

    public:
        /**
         *  \brief alias for Tail
         *  \req not empty ()
         */
        nonvirtual T back () const;

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
        virtual void       AddHead (ArgByValueType<value_type> item) = 0;
        virtual value_type RemoveTail ()                             = 0;
        virtual value_type Tail () const                             = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Deque.inl"

#endif /*_Stroika_Foundation_Containers_Deque_h_ */
