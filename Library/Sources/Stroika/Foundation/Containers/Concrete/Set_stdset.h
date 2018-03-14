/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Set.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Set_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_Set_stdset_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  Set_stdset requires its own traits (besides DefaultTraits::Set) because of the neeed for a compare function for std::set<>
                 */
                template <typename T, typename EQUALS_COMPARER = typename DefaultTraits::Set<T>::EqualsCompareFunctionType, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
                struct Set_stdset_DefaultTraits : DefaultTraits::Set<T, EQUALS_COMPARER> {
                    /**
                     */
                    using WellOrderCompareFunctionType = WELL_ORDER_COMPARER;

                    /**
                     */
                    RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);
                };

                /**
                 *  \brief   Set_stdset<KEY_TYPE, VALUE_TYPE, TRAITS> is an std::set-based concrete implementation of the Set<T, TRAITS> container pattern.
                 *
                 *  \note   \em Implemenation   Just indirect to SortedSet_stdset<>, which in turn mostly indirects to std::set<T>
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T, typename TRAITS = Set_stdset_DefaultTraits<T>>
                class Set_stdset : public Set<T, typename TRAITS::SetTraitsType> {
                private:
                    using inherited = Set<T, typename TRAITS::SetTraitsType>;

                public:
                    /**
                     */
                    using TraitsType = TRAITS;

                public:
                    /**
                     */
                    using WellOrderCompareFunctionType = typename TraitsType::WellOrderCompareFunctionType;

                public:
                    RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);

                public:
                    /**
                     */
                    Set_stdset ();
                    Set_stdset (const Set_stdset<T, TRAITS>& src);
                    Set_stdset (const std::initializer_list<T>& src);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const Set_stdset<T>*>::value>::type>
                    explicit Set_stdset (const CONTAINER_OF_T& src);
                    template <typename COPY_FROM_ITERATOR_T>
                    explicit Set_stdset (COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end);

                public:
                    /**
                     */
                    nonvirtual Set_stdset<T, TRAITS>& operator= (const Set_stdset<T, TRAITS>& rhs);

                private:
                    class IImplRepBase_;
                    template <typename USE_COMPARER>
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
#include "Set_stdset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Set_stdset_h_ */
