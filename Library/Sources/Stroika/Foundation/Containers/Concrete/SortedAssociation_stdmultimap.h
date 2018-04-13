/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../SortedAssociation.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedAssociation_stdmultimap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedAssociation_stdmultimap_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 ***VERY ROUGH UNUSABLE DRAFT*
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
                 *  \brief   SortedAssociation_stdmultimap<Key,T> is an std::map-based concrete implementation of the SortedAssociation<Key,T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                class SortedAssociation_stdmultimap : public SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE> {
                private:
                    using inherited = SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>;

                public:
                    /**
                     *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
                     */
                    SortedAssociation_stdmultimap ();
                    SortedAssociation_stdmultimap (const SortedAssociation_stdmultimap& src) = default;
                    template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_ADDABLE>::value && !std::is_convertible<const CONTAINER_OF_ADDABLE*, const SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>*>::value>::type>
                    SortedAssociation_stdmultimap (const CONTAINER_OF_ADDABLE& src);
                    template <typename COPY_FROM_ITERATOR_KEY_T>
                    SortedAssociation_stdmultimap (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    /**
                     */
                    nonvirtual SortedAssociation_stdmultimap& operator= (const SortedAssociation_stdmultimap& rhs) = default;

                private:
                    class IImplRep_;
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
#include "SortedAssociation_stdmultimap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedAssociation_stdmultimap_h_ */
