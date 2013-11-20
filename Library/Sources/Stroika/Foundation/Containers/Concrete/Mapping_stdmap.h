/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_

#include    "../../StroikaPreComp.h"

#include    "../Mapping.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  Mapping_stdmap requires its own traits (besides Mapping_DefaultTraits) because of the neeed for a compare function for std::map<>
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = Common::ComparerWithEquals<KEY_TYPE>, typename VALUE_EQUALS_COMPARER = Common::ComparerWithEqualsOptionally<VALUE_TYPE>, typename KEY_WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<KEY_TYPE>>
                struct   Mapping_stdmap_DefaultTraits : Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER, VALUE_EQUALS_COMPARER> {
                    /**
                     */
                    typedef KEY_WELL_ORDER_COMPARER KeyWellOrderCompareFunctionType;

                    typedef Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER, VALUE_EQUALS_COMPARER>  MappingTraitsType;

                    RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);
                };


                /**
                 *  \brief   Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 *  \note   \em Implementation Details
                 *          This module is essentially identical to SortedMapping_stdmap, but making it dependent on SortedMapping<> creates
                 *          problems with circular dependencies - especially give how the default Mapping CTOR calls the factory class
                 *          which maps back to the _stdmap<> variant.
                 *
                 *          There maybe another (better) way, but this works.
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = Mapping_stdmap_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
                class   Mapping_stdmap : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> {
                private:
                    typedef     Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>  inherited;

                public:
                    /**
                     */
                    typedef TRAITS      TraitsType;

                public:
                    /**
                     */
                    typedef typename    TraitsType::KeyWellOrderCompareFunctionType KeyWellOrderCompareFunctionType;

                public:
                    RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);

                public:
                    /**
                     */
                    Mapping_stdmap ();
                    Mapping_stdmap (const Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& src);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Mapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& src);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_stdmap (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    /**
                     */
                    nonvirtual  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;

                private:
                    nonvirtual  const Rep_&  GetRep_ () const;
                    nonvirtual  Rep_&        GetRep_ ();
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
#include    "Mapping_stdmap.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Mapping_stdmap_h_ */
