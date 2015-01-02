/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_ReBin_inl_
#define _Stroika_Foundation_Math_ReBin_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "Common.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Math {
            namespace   ReBin {


                /*
                 ********************************************************************************
                 ********************** Math::ReBin::BasicDataDescriptor ************************
                 ********************************************************************************
                 */
                template    <typename X_TYPE, typename VALUE_TYPE>
                inline  BasicDataDescriptor<X_TYPE, VALUE_TYPE>::BasicDataDescriptor (const ValueType* bucketStart, const ValueType* bucketEnd, XType xStart, XType xEnd)
                    : fBucketDataStart_ (bucketStart)
                    , fBucketDataEnd_ (bucketEnd)
                    , fXStart_ (xStart)
                    , fXEnd_ (xEnd)
                {
                    RequireNotNull (bucketStart);
                    RequireNotNull (bucketEnd);
                    Require (bucketStart < bucketEnd);
                    Require (xStart < xEnd);
                }
                template    <typename X_TYPE, typename VALUE_TYPE>
                inline  typename BasicDataDescriptor<X_TYPE, VALUE_TYPE>::BucketIndexType  BasicDataDescriptor<X_TYPE, VALUE_TYPE>::GetBucketCount () const
                {
                    BucketIndexType result = fBucketDataEnd_ - fBucketDataStart_;
                    Ensure (result > 0);
                    return result;
                }
                template    <typename X_TYPE, typename VALUE_TYPE>
                inline  Traversal::Range<X_TYPE>   BasicDataDescriptor<X_TYPE, VALUE_TYPE>::GetBucketRange (BucketIndexType bucket) const
                {
                    using Traversal::Range;
                    Require (bucket < GetBucketCount ());
                    X_TYPE  kDelta_ = (fXEnd_ - fXStart_) * (static_cast<X_TYPE> (1) / static_cast<X_TYPE> (GetBucketCount ()));
                    X_TYPE  s = fXStart_ + (static_cast<X_TYPE> (bucket) * kDelta_);
                    return Range<X_TYPE> (s, s + kDelta_, Traversal::Openness::eClosed, Traversal::Openness::eOpen);
                }
                template    <typename X_TYPE, typename VALUE_TYPE>
                Traversal::DiscreteRange<typename BasicDataDescriptor<X_TYPE, VALUE_TYPE>::BucketIndexType> BasicDataDescriptor<X_TYPE, VALUE_TYPE>::GetMappedBucketRange (const Traversal::Range<X_TYPE>& xrange) const
                {
                    using Traversal::DiscreteRange;
                    if (xrange.GetUpperBound () < fXStart_) {
                        return DiscreteRange<BucketIndexType> ();
                    }
                    if (xrange.GetLowerBound () > fXEnd_) {
                        return DiscreteRange<BucketIndexType> ();
                    }

                    X_TYPE  kDelta_ = (fXEnd_ - fXStart_) * (static_cast<X_TYPE> (1) / static_cast<X_TYPE> (GetBucketCount ()));

                    // very roughly (quick hack)
                    X_TYPE bucketLowerBound = (xrange.GetLowerBound () - fXStart_) / kDelta_;
                    X_TYPE bucketUpperBound = (xrange.GetUpperBound () - fXStart_) / kDelta_;

                    bucketLowerBound = (bucketLowerBound < 0) ? 0 : bucketLowerBound;
                    bucketUpperBound = (bucketUpperBound < 0) ? 0 : bucketUpperBound;

                    BucketIndexType    bucketLB = Math::PinInRange<BucketIndexType> (static_cast<BucketIndexType> (floor (bucketLowerBound)), 0, GetBucketCount () - 1);
                    BucketIndexType    bucketUB = Math::PinInRange<BucketIndexType> (static_cast<BucketIndexType> (ceil (bucketUpperBound)), bucketLB, GetBucketCount () - 1);

                    return DiscreteRange<BucketIndexType> (bucketLB, bucketUB);
                }
                template    <typename X_TYPE, typename VALUE_TYPE>
                inline  VALUE_TYPE  BasicDataDescriptor<X_TYPE, VALUE_TYPE>::GetValue (BucketIndexType bucket) const
                {
                    Require (bucket < GetBucketCount ());
                    return fBucketDataStart_[bucket];
                }
#if qCompilerAndStdLib_constexpr_Buggy
                template    <typename X_TYPE, typename VALUE_TYPE>
                const VALUE_TYPE BasicDataDescriptor<X_TYPE, VALUE_TYPE>::kZero = 0;
#endif


                /*
                 ********************************************************************************
                 ***************** Math::ReBin::UpdatableDataDescriptor *************************
                 ********************************************************************************
                 */
                template    <typename X_TYPE, typename VALUE_TYPE>
                inline  UpdatableDataDescriptor<X_TYPE, VALUE_TYPE>::UpdatableDataDescriptor (VALUE_TYPE* bucketStart, VALUE_TYPE* bucketEnd, X_TYPE xStart, X_TYPE xEnd)
                    : inherited (bucketStart, bucketEnd, xStart, xEnd)
                {
                }
                template    <typename X_TYPE, typename VALUE_TYPE>
                inline  void  UpdatableDataDescriptor<X_TYPE, VALUE_TYPE>::AccumulateValue (typename inherited::BucketIndexType bucket, VALUE_TYPE delta)
                {
                    Require (bucket < inherited::GetBucketCount ());
                    VALUE_TYPE*  updatableStart = const_cast<VALUE_TYPE*> (inherited::fBucketDataStart_);
                    updatableStart[bucket] += delta;
                }
                template    <typename X_TYPE, typename VALUE_TYPE>
                inline  void    UpdatableDataDescriptor<X_TYPE, VALUE_TYPE>::ZeroBuckets ()
                {
                    VALUE_TYPE*  updatableStart = const_cast<VALUE_TYPE*> (inherited::fBucketDataStart_);
                    for (VALUE_TYPE* i = updatableStart; i != inherited::fBucketDataEnd_; ++i) {
                        *i = inherited::kZero;
                    }
                }


                /*
                 ********************************************************************************
                 ********************************** Math::ReBin *********************************
                 ********************************************************************************
                 */
                template    <typename   SRC_DATA_DESCRIPTOR, typename TRG_DATA_DESCRIPTOR>
                void    ReBin (
                    const SRC_DATA_DESCRIPTOR& srcData,
                    TRG_DATA_DESCRIPTOR* trgData
                )
                {
                    RequireNotNull (trgData);

                    using   namespace Traversal;
                    /*
                     *  OLD OBSOLETE Algorithm:
                     *      Two iterators - one marking (start/end of target buckets), and one marking current
                     *      src bucket. Iterate over outer buckets. Move contents to new bucket. And adjust new
                     *      iterators. When they overlap and must advance - proportionally add to bucket,
                     *      advance and add rest to next target bucket.
                     *
                     *      This is probably more efficent than the algorithm below, but trickier to generalize
                     *      so we can have differently scaled source buckets.
                     */

                    /*
                     *  By default, zero all target buckets, but you could override that in the TRG argument
                     *  to avoid zeroing (say to accumulate multiple sources)
                     */
                    trgData->ZeroBuckets ();

                    /*
                     *  x               0    1    2    3    4    5
                     *
                     *  SRC-BUCKETS:    |    |    |    |    |    |
                     *  TRG-BUCKETS:    |      |      |      |
                     *
                     *  Iterate over each source bucket. It represnts data from its sourceXStart, to srcXEnd (bucket X domain)
                     *  all data spread evently.
                     *
                     *  See where that start x/endx map to in y buckets, and spread it over them. Assume that in the TARGET
                     *  the buckets are contiguous.
                     *
                     *  For each one source bucket, put part into current ti (proportional), and put reset into next
                     *  ti (proportional).
                     */
                    size_t      srcBucketCount = srcData.GetBucketCount ();
                    for (size_t srcBucketIdx = 0; srcBucketIdx < srcBucketCount; ++srcBucketIdx) {
                        Range<typename SRC_DATA_DESCRIPTOR::XType>  curSrcBucketX       =   srcData.GetBucketRange(srcBucketIdx);
                        auto                                        curSrcBucketXWidth  =   curSrcBucketX.GetDistanceSpanned ();
                        typename SRC_DATA_DESCRIPTOR::ValueType     thisSrcBucketValue  =   srcData.GetValue(srcBucketIdx);

                        Assert (curSrcBucketXWidth >= 0);   // can ever be zero?

                        // Check special value of zero so we dont waste time spreading zeros around
                        if (thisSrcBucketValue != SRC_DATA_DESCRIPTOR::kZero and curSrcBucketXWidth != 0) {
                            /*
                             *  find range of target buckets to distribute value.
                             *
                             *  Each bucket returned may have little overall contribution from the source bucket. We look at
                             *  the degree of overlap.
                             */
                            for (auto targetBucket : trgData->GetMappedBucketRange (curSrcBucketX)) {
                                Range<typename SRC_DATA_DESCRIPTOR::XType>  trgBucketIntersectRange =   trgData->GetBucketRange(targetBucket).Intersection (curSrcBucketX);
                                auto                                        trgBucketXWidth         =   trgBucketIntersectRange.GetDistanceSpanned ();
                                trgData->AccumulateValue (targetBucket, thisSrcBucketValue * (trgBucketXWidth / curSrcBucketXWidth));
                            }
                        }
                    }
                }
                template    <typename SRC_BUCKET_TYPE, typename TRG_BUCKET_TYPE, typename X_OFFSET_TYPE>
                void    ReBin (
                    const SRC_BUCKET_TYPE* srcStart, const SRC_BUCKET_TYPE* srcEnd,
                    TRG_BUCKET_TYPE* trgStart, TRG_BUCKET_TYPE* trgEnd
                )
                {
                    using   SRC_DATA_DESCRIPTOR     =   BasicDataDescriptor<X_OFFSET_TYPE, SRC_BUCKET_TYPE>;
                    using   TRG_DATA_DESCRIPTOR     =   UpdatableDataDescriptor<X_OFFSET_TYPE, TRG_BUCKET_TYPE>;

                    SRC_DATA_DESCRIPTOR srcData (srcStart, srcEnd, 1, 2);
                    TRG_DATA_DESCRIPTOR trgData (trgStart, trgEnd, 1, 2);
                    ReBin (srcData, &trgData);
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Math_ReBin_inl_*/
