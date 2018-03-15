/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Set.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   Set_LinkedList<T, TRAITS> is an LinkedList-based concrete implementation of the Set<T, TRAITS> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename T, typename TRAITS = DefaultTraits::Set<T>>
                class Set_LinkedList : public Set<T, typename TRAITS::SetTraitsType> {
                private:
                    using inherited = Set<T, typename TRAITS::SetTraitsType>;

                public:
                    /**
                     */
                    Set_LinkedList ();
                    Set_LinkedList (const Set_LinkedList<T, TRAITS>& src);
                    Set_LinkedList (const std::initializer_list<T>& src);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const Set_LinkedList<T>*>::value>::type>
                    explicit Set_LinkedList (const CONTAINER_OF_T& src);
                    template <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Set_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    /**
                     */
                    nonvirtual Set_LinkedList<T, TRAITS>& operator= (const Set_LinkedList<T, TRAITS>& rhs);

                private:
                    class IImplRepBase_;
                    template <typename EQUALS_COMPARER>
                    class Rep_;

                private:
                    nonvirtual void AssertRepValidType_ () const;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Set_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_ */
