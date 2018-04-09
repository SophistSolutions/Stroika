/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../SortedMapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Cleanup IteratorRep_ code.
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   SortedMapping_stdmap<KEY_TYPE,MAPPED_VALUE_TYPE> is an std::map-based concrete implementation of the SortedMapping<KEY_TYPE,MAPPED_VALUE_TYPE> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                class SortedMapping_stdmap : public SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
                private:
                    using inherited = SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

                public:
                    SortedMapping_stdmap ();
                    template <typename KEY_INORDER_COMPARER, typename ENABLE_IF_IS_COMPARER = enable_if_t<Common::IsPotentiallyComparerRelation<KEY_INORDER_COMPARER> ()>>
                    explicit SortedMapping_stdmap (const KEY_INORDER_COMPARER& inorderComparer, ENABLE_IF_IS_COMPARER* = nullptr);
                    SortedMapping_stdmap (const SortedMapping_stdmap& src) = default;
                    template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value && not std::is_convertible<const CONTAINER_OF_PAIR_KEY_T*, const SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>*>::value>::type>
                    SortedMapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& src);
                    template <typename COPY_FROM_ITERATOR_KEYVALUE>
                    SortedMapping_stdmap (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

                public:
                    /**
                     */
                    nonvirtual SortedMapping_stdmap& operator= (const SortedMapping_stdmap& rhs) = default;

                private:
                    class IImplRepBase_;
                    template <typename KEY_INORDER_COMPARER>
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
#include "SortedMapping_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_ */
