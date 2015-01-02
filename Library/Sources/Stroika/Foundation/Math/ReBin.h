/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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


                /**
                 *  Utility to describe source data (bins) for use in the ReBin() API.
                 *
                 *  This is not needed explicitly for simple usage, but is just for complicated cases.
                 */
                template    <typename X_TYPE, typename VALUE_TYPE>
                class   BasicDataDescriptor {
                public:
                    using   BucketIndexType =   size_t;
                    using   XType           =   X_TYPE;
                    using   ValueType       =   VALUE_TYPE;

                public:
#if     qCompilerAndStdLib_constexpr_Buggy
                    static const ValueType kZero;
#else
                    static constexpr ValueType kZero { 0 };
#endif

                public:
                    BasicDataDescriptor (const ValueType* bucketStart, const ValueType* bucketEnd, XType xStart, XType xEnd);

                public:
                    nonvirtual  BucketIndexType  GetBucketCount () const;

                public:
                    nonvirtual  Traversal::Range<X_TYPE>   GetBucketRange (BucketIndexType bucket) const;

                public:
                    // for the given argument x-range, find the range of intersecting buckets
                    // this is assumed to be contiguous (for now)
                    nonvirtual  Traversal::DiscreteRange<BucketIndexType> GetMappedBucketRange(const Traversal::Range<X_TYPE>& xrange) const;

                public:
                    nonvirtual  VALUE_TYPE  GetValue (BucketIndexType bucket) const;

                protected:
                    const VALUE_TYPE*   fBucketDataStart_;
                    const VALUE_TYPE*   fBucketDataEnd_;
                    X_TYPE              fXStart_;
                    X_TYPE              fXEnd_;
                };


                /**
                 *  Utility to describe target data (bins) for use in the ReBin() API.
                 *  This is not needed explicitly for simple usage, but is just for complicated cases.
                 */
                template    <typename X_TYPE, typename VALUE_TYPE>
                class   UpdatableDataDescriptor : public BasicDataDescriptor<X_TYPE, VALUE_TYPE> {
                private:
                    using inherited = BasicDataDescriptor<X_TYPE, VALUE_TYPE>;
                public:
                    UpdatableDataDescriptor (VALUE_TYPE* bucketStart, VALUE_TYPE* bucketEnd, X_TYPE xStart, X_TYPE xEnd);

                public:
                    nonvirtual  void    AccumulateValue (typename inherited::BucketIndexType bucket, VALUE_TYPE delta);

                public:
                    nonvirtual  void    ZeroBuckets ();
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
                 *  The new bins could also - have been offset slightly versus the new bins (that is the zeroth
                 *  bin of the new set of bins need not start at the same x-value as the original set of bins).
                 *
                 *  Classically - this assumes the curve was fairly linear across the new set original set of bins.
                 *  As a future exercise, we may want to experiment  with different assumptions (like linear
                 *  up/down according to prev and successive bins?).
                 *
                 *  EXAMPLE:
                 *      uint32_t srcBinData[] = { 3, 5, 19, 2, 0, 0, 0 };
                 *      double  resultData[4];
                 *      ReBin (begin (srcBinData), end (srcBinData), begin (resultData), end (resultData));
                 *      VerifyTestResult (NearlyEquals ((3 + (5 * ((7.0 / 4.0) - 1))), resultData[0]));
                 *      VerifyTestResult (0 == resultData[3]);
                 *
                 *  EXAMPLE:
                 *      uint32_t srcBinData[] = { 3, 5, 19, 2 };
                 *      double  resultData[8];
                 *      ReBin (begin (srcBinData), end (srcBinData), begin (resultData), end (resultData));
                 *      VerifyTestResult (NearlyEquals (1.5, resultData[0]));
                 *      VerifyTestResult (NearlyEquals (1.5, resultData[1]));
                 *      VerifyTestResult (NearlyEquals (2.5, resultData[2]));
                 *      VerifyTestResult (NearlyEquals (2.5, resultData[3]));
                 *
                 *  EXAMPLE:
                 *      // Shifting by partial bin (1/10 of 4, or by 2/5)
                 *      uint32_t srcBinData[] = { 3, 5, 19, 2 };
                 *      double  resultData[4];
                 *      using   SRC_DATA_DESCRIPTOR     =   BasicDataDescriptor<double, uint32_t>;
                 *      using   TRG_DATA_DESCRIPTOR     =   UpdatableDataDescriptor<double, double>;
                 *      SRC_DATA_DESCRIPTOR srcData (srcStart, srcEnd, 0, 10);
                 *      TRG_DATA_DESCRIPTOR trgData (trgStart, trgEnd, 1, 11);
                 *      ReBin (srcData, &trgData);
                 *
                 *      // In src, srcBinData[0] extends from 0 .. 10/4 (ie 0..2.5), and srcBinData[1] extends from 2.5 to 5.0
                 *      // In target, resultData[0] extends from 1 to (11-1)/4 + 1, or 1 to 3.5
                 *      // SO... resultData[0] = 3 * (pct of src bucket 0) + 5 * (pct of src bucket 1)
                 *      //                     = 3 * (1.5/2.5) + 5 * (1/2.5) = 1.8 + 2
                 *      VerifyTestResult (NearlyEquals (3.8, resultData[0]));
                 *
                 *  @todo SHOW MORE COMPLICATED EXAMPLE BASED ON:...
                 *      struct SRC_DATA_DESCRIPTOR : BasicDataDescriptor<double, double> {
                 *          ...
                 *          static  Range<XType>   GetBucketRange (unsigned int bucket) {
                 *                  return fMapper_->ToFrequency (fMapper_->BinToAtoDValue (bucket));
                 *              }
                 *          }
                 *      };
                 *      using TRG_DATA_DESCRIPTOR = UpdatableDataDescriptor<double, double>;
                 *
                 */
                template    <typename   SRC_DATA_DESCRIPTOR, typename TRG_DATA_DESCRIPTOR>
                void    ReBin (
                    const SRC_DATA_DESCRIPTOR& srcData,
                    TRG_DATA_DESCRIPTOR* trgData
                );
                template    <typename SRC_BUCKET_TYPE, typename TRG_BUCKET_TYPE, typename X_OFFSET_TYPE = double>
                void    ReBin (
                    const SRC_BUCKET_TYPE* srcStart, const SRC_BUCKET_TYPE* srcEnd,
                    TRG_BUCKET_TYPE* trgStart, TRG_BUCKET_TYPE* trgEnd
                );


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
