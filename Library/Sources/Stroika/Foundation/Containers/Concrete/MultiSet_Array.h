/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../MultiSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_ 1

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals ()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
                class MultiSet_Array : public MultiSet<T, TRAITS> {
                private:
                    using inherited = MultiSet<T, TRAITS>;

                public:
                    MultiSet_Array ();
                    template <typename EQUALS_COMPARER, typename ENABLE_IF_IS_COMPARER = enable_if_t<Configuration::is_callable<EQUALS_COMPARER>::value>>
                    explicit MultiSet_Array (const EQUALS_COMPARER& equalsComparer, ENABLE_IF_IS_COMPARER* = nullptr);
                    MultiSet_Array (const MultiSet_Array& src) = default;
                    MultiSet_Array (MultiSet_Array&& src)      = default;
                    MultiSet_Array (const MultiSet<T, TRAITS>& src);
                    MultiSet_Array (const std::initializer_list<T>& src);
                    MultiSet_Array (const std::initializer_list<CountedValue<T>>& src);
                    template <typename COPY_FROM_ITERATOR>
                    MultiSet_Array (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

                public:
                    /**
                     */
                    nonvirtual MultiSet_Array& operator= (const MultiSet_Array& rhs) = default;

                public:
                    nonvirtual size_t GetCapacity () const;
                    nonvirtual void   SetCapacity (size_t slotsAlloced);

                public:
                    nonvirtual void Compact ();

                public:
                    /**
                     *  STL-ish alias for GetCapacity ();
                     */
                    nonvirtual size_t capacity () const;

                public:
                    /**
                     *  STL-ish alias for SetCapacity ();
                     */
                    nonvirtual void reserve (size_t slotsAlloced);

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
#include "MultiSet_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_*/
