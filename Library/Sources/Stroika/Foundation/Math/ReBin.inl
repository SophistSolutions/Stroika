/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_ReBin_inl_
#define _Stroika_Foundation_Math_ReBin_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"
#include "../Traversal/Generator.h"
#include "../Traversal/Partition.h"

#include "Common.h"

namespace Stroika {
    namespace Foundation {
        namespace Math {
            namespace ReBin {

                /*
                 ********************************************************************************
                 ********************** Math::ReBin::DataDescriptorBase *************************
                 ********************************************************************************
                 */
                template <typename X_TYPE, typename VALUE_TYPE>
                inline bool DataDescriptorBase<X_TYPE, VALUE_TYPE>::RangeElementsNearlyEqual (XType lhs, XType rhs)
                {
                    return NearlyEquals (lhs, rhs);
                }

                /*
                 ********************************************************************************
                 ********************** Math::ReBin::BasicDataDescriptor ************************
                 ********************************************************************************
                 */
                template <typename X_TYPE, typename VALUE_TYPE>
                inline BasicDataDescriptor<X_TYPE, VALUE_TYPE>::BasicDataDescriptor (const ValueType* bucketStart, const ValueType* bucketEnd, XType xStart, XType xEnd)
                    : _fBucketDataStart (bucketStart)
                    , _fBucketDataEnd (bucketEnd)
                    , fXStart_ (xStart)
                    , fXEnd_ (xEnd)
                {
                    RequireNotNull (bucketStart);
                    RequireNotNull (bucketEnd);
                    Require (bucketStart < bucketEnd);
                    Require (xStart < xEnd);
                }
                template <typename X_TYPE, typename VALUE_TYPE>
                inline typename BasicDataDescriptor<X_TYPE, VALUE_TYPE>::BucketIndexType BasicDataDescriptor<X_TYPE, VALUE_TYPE>::GetBucketCount () const
                {
                    BucketIndexType result = _fBucketDataEnd - _fBucketDataStart;
                    Ensure (result > 0);
                    return result;
                }
                template <typename X_TYPE, typename VALUE_TYPE>
                inline auto BasicDataDescriptor<X_TYPE, VALUE_TYPE>::GetBucketRange (BucketIndexType bucket) const -> Traversal::Range<XType>
                {
                    using Traversal::Range;
                    Require (bucket < GetBucketCount ());
                    X_TYPE kDelta_ = (fXEnd_ - fXStart_) * (static_cast<X_TYPE> (1) / static_cast<X_TYPE> (GetBucketCount ()));
                    X_TYPE s       = fXStart_ + (static_cast<X_TYPE> (bucket) * kDelta_);
                    return Range<X_TYPE> (s, s + kDelta_, Traversal::Openness::eClosed, Traversal::Openness::eOpen);
                }
                template <typename X_TYPE, typename VALUE_TYPE>
                auto BasicDataDescriptor<X_TYPE, VALUE_TYPE>::GetIntersectingBuckets (const Traversal::Range<XType>& xrange) const -> Containers::Set<BucketIndexType>
                {
                    using Traversal::DiscreteRange;
                    if (xrange.GetUpperBound () < fXStart_) {
                        return Containers::Set<BucketIndexType> ();
                    }
                    if (xrange.GetLowerBound () > fXEnd_) {
                        return Containers::Set<BucketIndexType> ();
                    }

                    X_TYPE kDelta_ = (fXEnd_ - fXStart_) * (static_cast<X_TYPE> (1) / static_cast<X_TYPE> (GetBucketCount ()));

                    // very roughly (quick hack)
                    X_TYPE bucketLowerBound = (xrange.GetLowerBound () - fXStart_) / kDelta_;
                    X_TYPE bucketUpperBound = (xrange.GetUpperBound () - fXStart_) / kDelta_;

                    bucketLowerBound = (bucketLowerBound < 0) ? 0 : bucketLowerBound;
                    bucketUpperBound = (bucketUpperBound < 0) ? 0 : bucketUpperBound;

                    BucketIndexType bucketLB = Math::PinInRange<BucketIndexType> (static_cast<BucketIndexType> (floor (bucketLowerBound)), 0, GetBucketCount () - 1);
                    BucketIndexType bucketUB = Math::PinInRange<BucketIndexType> (static_cast<BucketIndexType> (ceil (bucketUpperBound)), bucketLB, GetBucketCount () - 1);

                    return Containers::Set<BucketIndexType> (DiscreteRange<BucketIndexType> (bucketLB, bucketUB).Elements ());
                }
                template <typename X_TYPE, typename VALUE_TYPE>
                inline auto BasicDataDescriptor<X_TYPE, VALUE_TYPE>::GetValue (BucketIndexType bucket) const -> ValueType
                {
                    Require (bucket < GetBucketCount ());
                    return _fBucketDataStart[bucket];
                }
                template <typename X_TYPE, typename VALUE_TYPE>
                inline auto BasicDataDescriptor<X_TYPE, VALUE_TYPE>::GetValues () const -> Containers::Sequence<ValueType>
                {
                    return Containers::Sequence<ValueType> (_fBucketDataStart, _fBucketDataEnd);
                }

                /*
                 ********************************************************************************
                 ***************** Math::ReBin::UpdatableDataDescriptor *************************
                 ********************************************************************************
                 */
                template <typename X_TYPE, typename VALUE_TYPE>
                inline UpdatableDataDescriptor<X_TYPE, VALUE_TYPE>::UpdatableDataDescriptor (VALUE_TYPE* bucketStart, VALUE_TYPE* bucketEnd, X_TYPE xStart, X_TYPE xEnd)
                    : inherited (bucketStart, bucketEnd, xStart, xEnd)
                {
                }
                template <typename X_TYPE, typename VALUE_TYPE>
                inline void UpdatableDataDescriptor<X_TYPE, VALUE_TYPE>::AccumulateValue (typename inherited::BucketIndexType bucket, VALUE_TYPE delta)
                {
                    Require (bucket < inherited::GetBucketCount ());
                    VALUE_TYPE* updatableStart = const_cast<VALUE_TYPE*> (inherited::_fBucketDataStart);
                    updatableStart[bucket] += delta;
                }
                template <typename X_TYPE, typename VALUE_TYPE>
                inline void UpdatableDataDescriptor<X_TYPE, VALUE_TYPE>::clear ()
                {
                    VALUE_TYPE* updatableStart = const_cast<VALUE_TYPE*> (inherited::_fBucketDataStart);
                    for (VALUE_TYPE* i = updatableStart; i != inherited::_fBucketDataEnd; ++i) {
                        *i = inherited::kNullValue;
                    }
                }

                /*
                 ********************************************************************************
                 ************************************* PRIVATE_ *********************************
                 ********************************************************************************
                 */
                namespace PRIVATE_ {
                    template <typename DATA_DESCRIPTOR_TYPE>
                    void CheckRebinDataDescriptorInvariant_ (const DATA_DESCRIPTOR_TYPE& d)
                    {
#if qDebug
                        using namespace Traversal;
                        using Memory::Optional;
                        using BucketIndexType = typename DATA_DESCRIPTOR_TYPE::BucketIndexType;
                        using XType           = typename DATA_DESCRIPTOR_TYPE::XType;
                        auto myContext        = make_shared<BucketIndexType> (0);
                        auto bucketCount      = d.GetBucketCount ();
                        auto getNext          = [myContext, bucketCount, d]() -> Optional<Range<XType>> {
                            /*
                             * Intentionally skip empty range elements, as legal in ReBin () - but which make
                             * the set not technically a partition.
                             */
                            Optional<Range<XType>> result;
                            while (result.IsMissing () and *myContext < bucketCount) {
                                Range<XType> tmp{d.GetBucketRange (*myContext)};
                                if (not tmp.empty ()) {
                                    result = tmp;
                                }
                                (*myContext)++;
                            }
                            return result;
                        };
                        Assert (IsPartition (CreateGenerator<Range<XType>> (getNext), DATA_DESCRIPTOR_TYPE::RangeElementsNearlyEqual));
#endif
                    }
                }

                /*
                 ********************************************************************************
                 ********************************** Math::ReBin *********************************
                 ********************************************************************************
                 */
                template <typename SRC_DATA_DESCRIPTOR, typename TRG_DATA_DESCRIPTOR>
                void ReBin (
                    const SRC_DATA_DESCRIPTOR& srcData,
                    TRG_DATA_DESCRIPTOR*       trgData)
                {
                    RequireNotNull (trgData);

                    using namespace Traversal;
                    using BucketIndexType = typename SRC_DATA_DESCRIPTOR::BucketIndexType;

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

#if qDebug
                    PRIVATE_::CheckRebinDataDescriptorInvariant_ (srcData);
                    PRIVATE_::CheckRebinDataDescriptorInvariant_ (*trgData);
#endif

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
                    BucketIndexType srcBucketCount = srcData.GetBucketCount ();
                    for (BucketIndexType srcBucketIdx = 0; srcBucketIdx < srcBucketCount; ++srcBucketIdx) {
                        Range<typename SRC_DATA_DESCRIPTOR::XType> curSrcBucketX      = srcData.GetBucketRange (srcBucketIdx);
                        auto                                       curSrcBucketXWidth = curSrcBucketX.GetDistanceSpanned ();
                        typename SRC_DATA_DESCRIPTOR::ValueType    thisSrcBucketValue = srcData.GetValue (srcBucketIdx);

                        Assert (curSrcBucketXWidth >= 0); // can ever be zero?

                        // Check special value of zero so we dont waste time spreading zeros around
                        if (thisSrcBucketValue != SRC_DATA_DESCRIPTOR::kNullValue and curSrcBucketXWidth != 0) {
                            /*
                             *  find range of target buckets to distribute value.
                             *
                             *  Each bucket returned may have little overall contribution from the source bucket. We look at
                             *  the degree of overlap.
                             */
                            for (auto targetBucket : trgData->GetIntersectingBuckets (curSrcBucketX)) {
                                Range<typename SRC_DATA_DESCRIPTOR::XType> trgBucketIntersectRange = trgData->GetBucketRange (targetBucket).Intersection (curSrcBucketX);
                                auto                                       trgBucketXWidth         = trgBucketIntersectRange.GetDistanceSpanned ();
                                trgData->AccumulateValue (targetBucket, thisSrcBucketValue * (trgBucketXWidth / curSrcBucketXWidth));
                            }
                        }
                    }
                }
                template <typename SRC_BUCKET_TYPE, typename TRG_BUCKET_TYPE, typename X_OFFSET_TYPE>
                void ReBin (
                    const SRC_BUCKET_TYPE* srcStart, const SRC_BUCKET_TYPE* srcEnd,
                    TRG_BUCKET_TYPE* trgStart, TRG_BUCKET_TYPE* trgEnd)
                {
                    using SRC_DATA_DESCRIPTOR = BasicDataDescriptor<X_OFFSET_TYPE, SRC_BUCKET_TYPE>;
                    using TRG_DATA_DESCRIPTOR = UpdatableDataDescriptor<X_OFFSET_TYPE, TRG_BUCKET_TYPE>;

                    SRC_DATA_DESCRIPTOR srcData (srcStart, srcEnd, 1, 2);
                    TRG_DATA_DESCRIPTOR trgData (trgStart, trgEnd, 1, 2);
                    trgData.clear (); // zero all the target buckets since this is a simple-usage variant and thats typically what will be wanted
                    ReBin (srcData, &trgData);
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Math_ReBin_inl_*/
