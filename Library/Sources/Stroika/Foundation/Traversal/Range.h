/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Range_h_
#define _Stroika_Foundation_Traversal_Range_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"

#include    "Iterator.h"



/**
 *  \file
 *
 *  STATUS:     VERY PRELIMINARY DRAFT - No WHERE NEAR COMPILING/TESTABLE.
 *
 *  NOTES:      INSPIRED BY READING ABOUT RUBY, but in the end, mostly
 *              based on HealthFrame's DateRangeType/DateTimeRangeType code.
 *
 *  TODO:
 *          @todo   When this works with basic numbers, then add specializations to the Time module so that
 *                  this stuff all works with dates, and datetime etc.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            template    <typename T>
            struct  DefaultRangeTraits {
                typedef ssize_t SignedDifferenceType;
                typedef size_t  UnsignedDifferenceType;

                // add values for min/max
                DEFINE_CONSTEXPR_CONSTANT(SignedDifferenceType, kMin, -100);    // use limit<T> stuff
                DEFINE_CONSTEXPR_CONSTANT(SignedDifferenceType, kMax, 100); // use limit<T> stuff
            };


            /**
             */
            template    <typename T, typename TRAITS = DefaultRangeTraits<T>>
            class  Range {
            public:
                /**
                 */
                typedef T   ElementType;

            public:
                explicit Range (const Memory::Optional<T>& min, const Memory::Optional<T>& max);


            public:
                /**
                 */
                nonvirtual  bool    Contains (T v) const;

            public:
                /**
                 */
                nonvirtual  bool    Overlaps (const Range<T, TRAITS>& v) const;

            public:
                /**
                 */
                nonvirtual  Range<T, TRAITS> Intersection (const Range<T, TRAITS>& v) const;

            public:
                /**
                 * if two regions are disjoint, this can encompass a larger region than the actual union would
                 */
                nonvirtual  Range<T, TRAITS> ExpandedUnion (const Range<T, TRAITS>& v) const;

            public:
                /**
                 */
                nonvirtual  bool    GetMin () const;

            public:
                /**
                 */
                nonvirtual  bool    GetMax () const;

            public:
                /**
                 */
                nonvirtual  bool    GetEffectiveMin () const;

            public:
                /**
                 */
                nonvirtual  bool    GetEffectiveMax () const;

            private:
                Memory::Optional<T>    fMin_;
                T                      fEffectiveMin_;
                Memory::Optional<T>    fMax_;
                T                      fEffectiveMax_;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Range.inl"

#endif  /*_Stroika_Foundation_Traversal_Range_h_ */
