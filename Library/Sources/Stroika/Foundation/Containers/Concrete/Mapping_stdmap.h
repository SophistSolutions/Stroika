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
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                class Mapping_stdmap : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
                private:
                    using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

                public:
                    /**
                     */
                    //using KeyWellOrderCompareFunctionType = typename TraitsType::KeyWellOrderCompareFunctionType;

                public:
                    /**
                     */
                    Mapping_stdmap ();
                    Mapping_stdmap (const Mapping_stdmap& src) = default;
                    template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value && !std::is_convertible<const CONTAINER_OF_PAIR_KEY_T*, const Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>*>::value>::type>
                    explicit Mapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& src);
                    template <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_stdmap (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    /**
                     */
                    nonvirtual Mapping_stdmap& operator= (const Mapping_stdmap& rhs) = default;

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
