/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_Tally_stdmap_h_

#include    "../../StroikaPreComp.h"

#include    "../Tally.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo   MUST re-implement (clone SortedTally_stdmap<>) code to avoid deadly include empbrace so we
 *              have the option to use this in the factory.
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
                    Tally_stdmap (const std::initializer_list<T>& s);
                    Tally_stdmap (const std::initializer_list<TallyEntry<T>>& s);
                    Tally_stdmap (const T* start, const T* end);

                public:
                    nonvirtual  Tally_stdmap<T, TRAITS>& operator= (const Tally_stdmap<T, TRAITS>& rhs);

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class Rep_;

                private:
                    nonvirtual  const Rep_& GetRep_ () const;
                    nonvirtual  Rep_&       GetRep_ ();
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
