/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Mapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  Mapping_stdmap requires its own traits (besides DefaultTraits::Mapping) because of the neeed for a compare function for std::map<>
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename KEY_EQUALS_COMPARER = typename DefaultTraits::Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::KeyEqualsCompareFunctionType, typename KEY_WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<KEY_TYPE>>
                struct Mapping_stdmap_DefaultTraits : DefaultTraits::Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_EQUALS_COMPARER> {
                    /**
                     */
                    using KeyWellOrderCompareFunctionType = KEY_WELL_ORDER_COMPARER;

                    RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);
                };

                /**
                 *  \brief   Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 *  \note   \em Implementation Details
                 *          This module is essentially identical to SortedMapping_stdmap, but making it dependent on SortedMapping<> creates
                 *          problems with circular dependencies - especially give how the default Mapping CTOR calls the factory class
                 *          which maps back to the _stdmap<> variant.
                 *
                 *          There maybe another (better) way, but this works.
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS = Mapping_stdmap_DefaultTraits<KEY_TYPE, MAPPED_VALUE_TYPE>>
                class Mapping_stdmap : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> {
                private:
                    using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType>;

                public:
                    /**
                     */
                    using TraitsType = TRAITS;

                public:
                    /**
                     */
                    using KeyWellOrderCompareFunctionType = typename TraitsType::KeyWellOrderCompareFunctionType;

                public:
                    RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);

                public:
                    /**
                     */
                    Mapping_stdmap ();
                    Mapping_stdmap (const Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>& src);
                    template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value && !std::is_convertible<const CONTAINER_OF_PAIR_KEY_T*, const Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>*>::value>::type>
                    explicit Mapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& src);
                    template <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_stdmap (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    /**
                     */
                    nonvirtual Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>& operator= (const Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>& rhs) = default;

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
#include "Mapping_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_ */
