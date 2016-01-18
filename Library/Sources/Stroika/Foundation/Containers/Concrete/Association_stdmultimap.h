/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_h_
#define _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_h_

#include    "../../StroikaPreComp.h"

#include    "../Association.h"



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
                 *  Association_stdmultimap requires its own traits (besides DefaultTraits::Association) because of the neeed for a compare function for std::map<>
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = typename DefaultTraits::Association<KEY_TYPE, VALUE_TYPE>::KeyEqualsCompareFunctionType, typename KEY_WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<KEY_TYPE>>
                struct   Association_stdmultimap_DefaultTraits : DefaultTraits::Association<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER> {
                    /**
                     */
                    using   KeyWellOrderCompareFunctionType =   KEY_WELL_ORDER_COMPARER;

                    RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);
                };


                /**
                 *  \brief   Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 *  \note   \em Implementation Details
                 *          This module is essentially identical to SortedAssociation_stdmultimap, but making it dependent on SortedAssociation<> creates
                 *          problems with circular dependencies - especially give how the default Association CTOR calls the factory class
                 *          which maps back to the _stdmap<> variant.
                 *
                 *          There maybe another (better) way, but this works.
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = Association_stdmultimap_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
                class   Association_stdmultimap : public Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType> {
                private:
                    using   inherited   =   Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>;

                public:
                    /**
                     */
                    using   TraitsType                      =   TRAITS;

                public:
                    /**
                     */
                    using   KeyWellOrderCompareFunctionType =   typename TraitsType::KeyWellOrderCompareFunctionType;

                public:
                    RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);

                public:
                    /**
                     */
                    Association_stdmultimap ();
                    Association_stdmultimap (const Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS>& src);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Association_stdmultimap (const CONTAINER_OF_PAIR_KEY_T& src);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Association_stdmultimap (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    /**
                     */
                    nonvirtual  Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) = default;

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                private:
                    nonvirtual  void    AssertRepValidType_ () const;
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
#include    "Association_stdmultimap.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Association_stdmultimap_h_ */
