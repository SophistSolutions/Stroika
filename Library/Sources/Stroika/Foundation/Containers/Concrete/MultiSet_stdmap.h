/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_

#include    "../../StroikaPreComp.h"

#include    "../MultiSet.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo   MUST re-implement (clone SortedMultiSet_stdmap<>) code to avoid deadly include empbrace so we
 *              have the option to use this in the factory.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  MultiSet_stdmap requires its own traits (besides MultiSet_DefaultTraits) because of the neeed for a compare function for std::map<>
                 */
                template    <typename T, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
                struct   MultiSet_stdmap_DefaultTraits : MultiSet_DefaultTraits <T, WELL_ORDER_COMPARER> {
                    /**
                     */
                    using   WellOrderCompareFunctionType    =   WELL_ORDER_COMPARER;

                    RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);
                };


                /**
                 */
                template    <typename T, typename TRAITS = MultiSet_stdmap_DefaultTraits<T>>
                class  MultiSet_stdmap : public MultiSet<T, TRAITS> {
                private:
                    using   inherited   =   MultiSet<T, TRAITS>;

                public:
                    /**
                     */
                    using   TraitsType                      =   TRAITS;

                public:
                    /**
                     */
                    using   WellOrderCompareFunctionType    =   typename TraitsType::WellOrderCompareFunctionType;

                public:
                    RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);

                public:
                    MultiSet_stdmap ();
                    template    <typename CONTAINER_OF_T>
                    MultiSet_stdmap (const CONTAINER_OF_T& src);
                    MultiSet_stdmap (const MultiSet_stdmap<T, TRAITS>& src);
                    MultiSet_stdmap (const initializer_list<T>& src);
                    MultiSet_stdmap (const initializer_list<MultiSetEntry<T>>& src);
                    MultiSet_stdmap (const T* start, const T* end);

                public:
                    nonvirtual  MultiSet_stdmap<T, TRAITS>& operator= (const MultiSet_stdmap<T, TRAITS>& rhs) = default;

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class Rep_;

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
#include    "MultiSet_stdmap.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_ */
