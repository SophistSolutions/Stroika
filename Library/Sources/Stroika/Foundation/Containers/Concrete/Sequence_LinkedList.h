/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Sequence.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_LinkedList<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Sequence_LinkedList : public Sequence<T> {
    private:
        using inherited = Sequence<T>;

    public:
        /**
         */
        Sequence_LinkedList ();
        Sequence_LinkedList (const Sequence_LinkedList& src) = default;
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Sequence_LinkedList<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        Sequence_LinkedList (CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_OF_T>
        explicit Sequence_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

    public:
        /**
         */
        nonvirtual Sequence_LinkedList<T>& operator= (const Sequence_LinkedList<T>& rhs) = default;

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

#include "Sequence_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_ */
