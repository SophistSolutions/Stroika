/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Deque_h_
#define _Stroika_Foundation_Containers_Deque_h_ 1

#include "../StroikaPreComp.h"

#include "Queue.h"

/*
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 *  TODO:
 *
 *      @todo   Consider  using the words front/back and push_front/pop_back etc - like with STL
 *
 */

namespace Stroika::Foundation::Containers {

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
     *  <<NOTE - FUTURE WORK - AND DON'T DOCUMENT DEFAULTIMPL HERE>>> - SEE FACTORY CODE
     *      We currently default to the circular array implementation, as it is
     *  fastest under most circumstances. One drawback to it is that it has
     *  unpredictable costs for an Add operation. DoubleLinkList is usually
     *  slower, but has very predictable costs.
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
    class Deque : public Queue<T> {
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

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (Enqueue)
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;

    protected:
        class _IRep;

    protected:
        using _IRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;

    public:
        /**
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        Deque ();
        Deque (Deque&& src) noexcept      = default;
        Deque (const Deque& src) noexcept = default;
        Deque (const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Deque<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Deque (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        Deque (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        /**
         */
        explicit Deque (_IRepSharedPtr&& src) noexcept;
        explicit Deque (const _IRepSharedPtr& src) noexcept;

    public:
        /**
         */
        nonvirtual Deque& operator= (Deque&& rhs) noexcept = default;
        nonvirtual Deque& operator= (const Deque& rhs) = default;

    public:
        /**
         *  \note mutates container
         */
        nonvirtual void AddHead (ArgByValueType<value_type> item);

    public:
        /**
         *  \note mutates container
         */
        nonvirtual value_type RemoveTail ();

    public:
        /**
         */
        nonvirtual value_type Tail () const;

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
