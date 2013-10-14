/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DiscreteRange_h_
#define _Stroika_Foundation_Traversal_DiscreteRange_h_  1

#include    "../StroikaPreComp.h"

#include    "Iterable.h"
#include    "Range.h"



/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  TODO:
 *      @todo   Conflict in names begin/end between Iterator and Range<> are INCONVENIENT and UNSESIRBALE.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {



#if 0
            template    <
            typename T,
                     T MIN = T::eSTART,
                     T MAX = T::eLAST,
                     typename SIGNED_DIFF_TYPE = int,
                     typename UNSIGNED_DIFF_TYPE = unsigned int
                     >
            struct  DefaultDiscreteRangeTraits_Enum  :
                    ExplicitRangeTraits<T, MIN, MAX, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
                static T GetNext (T n) {
                    return static_cast<T> (static_cast<int> (n) + 1);
                }
                typedef
                ExplicitRangeTraits<T, MIN, MAX, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
                RangeTraitsType
                ;
            };
#else
            template    <
            typename T,
#if     qSupportTemplateParamterOfNumericLimitsMinMax
                     T MIN = T::eSTART,
                     T MAX = T::eLAST,
#endif
                     typename SIGNED_DIFF_TYPE = int,
                     typename UNSIGNED_DIFF_TYPE = unsigned int
                     >
            struct  DefaultDiscreteRangeTraits_Enum  :
#if     qSupportTemplateParamterOfNumericLimitsMinMax
                    DefaultRangeTraits<T, MIN, MAX, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
#else
                    DefaultRangeTraits_Template_numericLimitsBWA<T, T::eSTART, T::eLAST, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
#endif
            {
                static T GetNext (T n) {
                    return static_cast<T> (static_cast<int> (n) + 1);
                }
                typedef
#if     qSupportTemplateParamterOfNumericLimitsMinMax
                DefaultRangeTraits<T, MIN, MAX, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
#else
                DefaultRangeTraits_Template_numericLimitsBWA<T, T::eSTART, T::eLAST, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>
#endif
                RangeTraitsType
                ;
            };
#endif


            /**
             */
            template    <
            typename T,
#if     qSupportTemplateParamterOfNumericLimitsMinMax
                     T MIN = numeric_limits<T>::min (),
                     T MAX = numeric_limits<T>::max (),
#endif
                     typename SIGNED_DIFF_TYPE = int,
                     typename UNSIGNED_DIFF_TYPE = unsigned int
                     >
            struct  DefaultDiscreteRangeTraits_Arithmetic :
                    DefaultRangeTraits <
                    T,
#if     qSupportTemplateParamterOfNumericLimitsMinMax
                    MIN,
                    MAX,
#endif
                    RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE
                    > {
                static T GetNext (T n) {
                    return static_cast<T> (static_cast<int> (n) + 1);
                }
                typedef
                DefaultRangeTraits <
                T,
#if     qSupportTemplateParamterOfNumericLimitsMinMax
                MIN,
                MAX,
#endif
                RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE
                >
                RangeTraitsType;
            };


            template    <
            typename T,
#if     qSupportTemplateParamterOfNumericLimitsMinMax
                     T MIN = numeric_limits<T>::min (),
                     T MAX = numeric_limits<T>::max (),
#endif
                     typename SIGNED_DIFF_TYPE = int,
                     typename UNSIGNED_DIFF_TYPE = unsigned int
                     >
            struct  DefaultDiscreteRangeTraits : conditional<is_enum<T>::value, DefaultDiscreteRangeTraits_Enum<T>, DefaultDiscreteRangeTraits_Arithmetic<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>>::type {
            };


            /**
             *  EXAMPLE USAGE:
             *      vector<int> v = DiscreteRange (1,10).As<vector<int>> ();
             *      // equiv to vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
             *
             *  OR:
             *      for (auto i : DiscreteRange (1,10)) {
             *          ...i = 1..10
             *      }
             */
            template    <typename T, typename TRAITS = DefaultDiscreteRangeTraits<T>>
            class  DiscreteRange : public Range<T, typename TRAITS::RangeTraitsType>, public Iterable<T> {
            private:
                typedef Range<T, typename TRAITS::RangeTraitsType>  inherited_RangeType;

            private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
            public:
#endif
                struct   MyIteratorRep_;
                struct   MyIteratableRep_;

            public:
                /**
                 */
                explicit DiscreteRange (const Memory::Optional<T>& begin, const Memory::Optional<T>& end);

            public:
                /**
                 */
                static  DiscreteRange<T, TRAITS> FullRange ();

            public:
                nonvirtual  bool empty () const;

            public:
                nonvirtual  Iterator<T> begin () const;
                nonvirtual  Iterator<T> end () const;
            };



        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "DiscreteRange.inl"

#endif  /*_Stroika_Foundation_Traversal_DiscreteRange_h_ */
