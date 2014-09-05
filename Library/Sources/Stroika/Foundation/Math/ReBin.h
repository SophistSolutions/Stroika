/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_ReBin_h_
#define _Stroika_Foundation_Math_ReBin_h_   1

#include    "../StroikaPreComp.h"

#include    "../Traversal/DiscreteRange.h"
#include    "../Traversal/Range.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *      @todo   Consider adding an x-offset, so that we logically re-bin, plus shift along the x-axis by
 *              a (given) offset (in units of src-bin widths).
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Math {
            namespace ReBin {


                template    <typename X_TYPE, typename VALUE_TYPE>
                class   BasicDataDescriptor {
                public:
                    using   BucketIndexType =   size_t;
                    using   XType           =   X_TYPE;
                    using   ValueType       =   VALUE_TYPE;

#if qCompilerAndStdLib_constexpr_Buggy
                    static const ValueType kZero;
#else
                    static constexpr ValueType kZero = 0;
#endif

                public:
                    BasicDataDescriptor (const ValueType* bucketStart, const ValueType* bucketEnd, XType xStart, XType xEnd);

                public:
                    BucketIndexType  GetBucketCount () const;
                    Traversal::Range<X_TYPE>   GetBucketRange (BucketIndexType bucket) const;

                protected:
                    const VALUE_TYPE* fBucketDataStart_;
                    const VALUE_TYPE* fBucketDataEnd_;
                    X_TYPE      fXStart_;
                    X_TYPE      fXEnd_;

                public:
                    // for the given argument x-range, find the range of intersecting buckets
                    // this is assumed to be contiguous (for now)
                    Traversal::DiscreteRange<BucketIndexType> GetMappedBucketRange(const Traversal::Range<X_TYPE>& xrange) const;

                public:
                    VALUE_TYPE  GetValue (BucketIndexType bucket) const;
                };

                /**
                 *  Take one array of counts (buckets/samples) - and stretch them (rebin them) to another number
                 *  of buckets.
                 *
                 *  Logically, think of a 2D graph, x versus y. The source buckets represent an approximation
                 *  of a real curve (y=f(x)). The buckets represent equally spaced measurements along the x-axis
                 *  and the corresponding y-axis value.
                 *
                 *  Re-binning means selecting a different (could be larger or smaller) bin count, and inferring the
                 *  curve from the source bins, and producing target bins that match that curve as best as possible.
                 *
                 *  The new bins could also - have been offset slightly versus the new bins (that is the zeroth bin of the
                 *  new set of bins need not start at the same x-value as the original set of bins).
                 *
                 *  Classically - this assumes the curve was fairly linear across the new set original set of bins.
                 *  As a future exercise, we may want to experiment  with different assumptions (like linear up/down according
                 *  to prev and successive bins?).
                 */
#if 0
                template    <typename SRC_BUCKET_TYPE, typename TRG_BUCKET_TYPE, typename X_OFFSET_TYPE = double>
                void    ReBin (
                    const SRC_BUCKET_TYPE* srcStart, const SRC_BUCKET_TYPE* srcEnd,
                    TRG_BUCKET_TYPE* trgStart, TRG_BUCKET_TYPE* trgEnd
                );
#endif
            }


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ReBin.inl"

#endif  /*_Stroika_Foundation_Math_ReBin_h_*/
