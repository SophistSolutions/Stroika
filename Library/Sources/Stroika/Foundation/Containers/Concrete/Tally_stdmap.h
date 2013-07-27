/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_Tally_stdmap_h_

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
                 *  Tally_stdmap requires its own traits (besides Tally_DefaultTraits) because of the neeed for a compare function for std::map<>
                 */
                template    <typename T, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
                struct   Tally_stdmap_DefaultTraits : Tally_DefaultTraits <T, WELL_ORDER_COMPARER> {
                    /**
                     */
                    typedef WELL_ORDER_COMPARER WellOrderCompareFunctionType;

                    RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);
                };


                /**
                 */
                template    <typename T, typename TRAITS = Tally_stdmap_DefaultTraits<T>>
                class  Tally_stdmap : public Tally<T, TRAITS> {
                private:
                    typedef Tally<T, TRAITS> inherited;

                public:
                    /**
                     */
                    typedef TRAITS      TraitsType;

                public:
                    /**
                     */
                    typedef typename    TraitsType::WellOrderCompareFunctionType WellOrderCompareFunctionType;

                public:
                    RequireConceptAppliesToTypeMemberOfClass(Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);

                public:
                    Tally_stdmap ();
                    template    <typename CONTAINER_OF_T>
                    Tally_stdmap (const CONTAINER_OF_T& src);
                    Tally_stdmap (const Tally_stdmap<T, TRAITS>& src);
                    Tally_stdmap (const T* start, const T* end);

                public:
                    nonvirtual  Tally_stdmap<T, TRAITS>& operator= (const Tally_stdmap<T, TRAITS>& rhs);
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
#include    "Tally_stdmap.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Tally_stdmap_h_ */
