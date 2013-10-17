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
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *      @todo   Conflict in names begin/end between Iterator and Range<> are INCONVENIENT and UNSESIRBALE.
 *
 *      @todo   Try to rewrite using Generator<> code... See if that simplifies things...
 *
 *      @todo   DefaultDiscreteRangeTraits_Enum and DefaultDiscreteRangeTraits_Integral<> should do better
 *              auto-computing the 'differnce' type
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             */
            template    <typename T, T MIN, T MAX, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            struct  ExplicitDiscreteRangeTraits  :
                    ExplicitRangeTraits_Integral<T, MIN, MAX, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
                static T GetNext (T n) {
                    return static_cast<T> (static_cast<int> (n) + 1);
                }
                typedef ExplicitRangeTraits_Integral<T, MIN, MAX, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>  RangeTraitsType;
            };


            /**
             *  DefaultDiscreteRangeTraits_Enum<> can be used to generate an automatic traits object (with bounds)
             *  if you've applied the Stroika_Define_Enum_Bounds() macro to the given enumeration.
             */
            template    <typename T>
            struct  DefaultDiscreteRangeTraits_Enum  : ExplicitDiscreteRangeTraits<T, T::eSTART, T::eLAST, int, unsigned int> {
            };


            /**
             */
#if     qSupportTemplateParamterOfNumericLimitsMinMax
            template    <typename T>
            struct  DefaultDiscreteRangeTraits_Integral  : ExplicitDiscreteRangeTraits<T, numeric_limits<T>::min (), numeric_limits<T>::max (), int, unsigned int> {
            };
#else
            template    <typename T>
            struct  DefaultDiscreteRangeTraits_Integral  : ExplicitDiscreteRangeTraits<T, 0, 1, int, unsigned int> {
#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                static  const T kMin    =   numeric_limits<T>::min ();
                static  const T kMax    =   numeric_limits<T>::max ();
#else
                static  const T kMin;
                static  const T kMax;
#endif
            };
#endif


            /**
             */
            template    <typename T>
            struct  DefaultDiscreteRangeTraits : conditional <
                    is_enum<T>::value,
                    DefaultDiscreteRangeTraits_Enum<T>,
                    DefaultDiscreteRangeTraits_Integral<T>
                    >::type {
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
            class   DiscreteRange : public Range<T, typename TRAITS::RangeTraitsType>, public Iterable<T> {
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
                explicit DiscreteRange (T begin, T end);
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
