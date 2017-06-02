/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../SortedMultiSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_ 1

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Get use of Private::IteratorImplHelper_ working
 *
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals ()
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
                template <typename T, typename TRAITS = DefaultTraits::SortedMultiSet<T>>
                class SortedMultiSet_stdmap : public SortedMultiSet<T, TRAITS> {
                private:
                    using inherited = SortedMultiSet<T, TRAITS>;

                public:
                    /**
                     */
                    SortedMultiSet_stdmap ();
                    SortedMultiSet_stdmap (const SortedMultiSet_stdmap<T, TRAITS>& src);
                    SortedMultiSet_stdmap (const std::initializer_list<T>& src);
                    SortedMultiSet_stdmap (const std::initializer_list<CountedValue<T>>& src);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const SortedMultiSet_stdmap<T, TRAITS>*>::value>::type>
                    explicit SortedMultiSet_stdmap (const CONTAINER_OF_T& src);
                    SortedMultiSet_stdmap (const T* start, const T* end);
                    SortedMultiSet_stdmap (const CountedValue<T>* start, const CountedValue<T>* end);

                public:
                    /**
                     */
                    nonvirtual SortedMultiSet_stdmap<T, TRAITS>& operator= (const SortedMultiSet_stdmap<T, TRAITS>& rhs) = default;

                private:
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
#include "SortedMultiSet_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_*/
