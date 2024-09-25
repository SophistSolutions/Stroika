/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Queue_Array_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Private/ArraySupport.h"
#include "Stroika/Foundation/Containers/Queue.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Add "MAX ENTRIES" feature - so throws/crashes when exceeded. Useful for a variety
 *              of queues, but I'm thinking mostly of Queue of incoming signals and need to avoid
 *              malloc call.
 *
 *      @todo   Redo using circular array strategy (we have this code in OLD stroika!!!)
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Queue_Array<T> is an Array-based concrete implementation of the Queue<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Queue_Array : public Private::ArrayBasedContainer<Queue_Array<T>, Queue<T>, false> {
    private:
        using inherited = Private::ArrayBasedContainer<Queue_Array<T>, Queue<T>, false>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \see docs on Queue<T> constructor
         */
        Queue_Array ();
        Queue_Array (Queue_Array&&) noexcept      = default;
        Queue_Array (const Queue_Array&) noexcept = default;
        Queue_Array (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Queue_Array<T>>)
        explicit Queue_Array (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Queue_Array{}
        {
            if constexpr (Common::IHasSizeMethod<ITERABLE_OF_ADDABLE>) {
                this->reserve (src.size ());
            }
            AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Queue_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Queue_Array& operator= (Queue_Array&&) noexcept = default;
        nonvirtual Queue_Array& operator= (const Queue_Array&)     = default;

    private:
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

#include "Queue_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Queue_Array_h_ */
