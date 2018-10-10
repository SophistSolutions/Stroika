/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
#include "../../StroikaPreComp.h"

#include "../Association.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Association_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Association_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 ***VERY ROUGH UNUSABLE DRAFT*
 *
 *  TODO:
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an LinkedList-based concrete implementation of the Association<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::AssociationTraitsType> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_LinkedList : public Association<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Association<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        Association_LinkedList ();
        Association_LinkedList (const Association_LinkedList& src) = default;
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_base_of_v<Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        explicit Association_LinkedList (CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_KEY_T>
        explicit Association_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

    public:
        nonvirtual Association_LinkedList& operator= (const Association_LinkedList& rhs) = default;

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

#include "Association_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_LinkedList_h_ */
