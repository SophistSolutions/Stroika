/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_ReBin_inl_
#define _Stroika_Foundation_Math_ReBin_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Math {


            /*
             ********************************************************************************
             ********************************** Math::ReBin *********************************
             ********************************************************************************
             */
            template    <typename SRC_BUCKET_TYPE, typename TRG_BUCKET_TYPE, typename X_OFFSET_TYPE>
            void    ReBin (
                const SRC_BUCKET_TYPE* srcStart, const SRC_BUCKET_TYPE* srcEnd,
                TRG_BUCKET_TYPE* trgStart, TRG_BUCKET_TYPE* trgEnd
            )
            {
                /*
                 *  Algorithm:
                 *      Two iterators - one marking (start/end of target buckets), and one marking current src bucket.
                 *      iterate over outer buckets. Move contents to new bucket. And adjust new iterators. When they overlap and must
                 *      advance - proportionally add to bucket, advance and add rest to next target bucket.
                 */
                size_t  nSrcBuckets =   srcEnd - srcStart;
                size_t  nTrgBuckets =   trgEnd - trgStart;
                Require (nSrcBuckets >= 1);
                Require (nTrgBuckets >= 1);
                Assert (nSrcBuckets >= nTrgBuckets);    // not a requirement, but otherwise NYI

                // zero target bins
                for (TRG_BUCKET_TYPE* i = trgStart; i != trgEnd; ++i) {
                    *i = 0;
                }

                X_OFFSET_TYPE       srcBucketsPerTrgBucket = static_cast<X_OFFSET_TYPE> (nSrcBuckets) / static_cast<X_OFFSET_TYPE> (nTrgBuckets);
                TRG_BUCKET_TYPE*    ti  =   trgStart;
                X_OFFSET_TYPE       xLeftInThisTrgBucket =   srcBucketsPerTrgBucket;

                /*
                 *  x               0    1    2    3    4    5
                 *
                 *  SRC-BUCKETS:    |    |    |    |    |    |
                 *  TRG-BUCKETS:    |      |      |      |
                 *
                 *  Filling target buckets (pre-initialized to zero). Major iteration over SRC buckets.
                 *  For each one (si), but part into current ti (proportional), and put reset into next
                 *  ti (proportional).
                 *
                 *      @todo - THIS CODE ASSUMES COUNT SRC BUCKETS > count TRG BUCKETS - MUST FIX!!!
                 *
                 */
                for (const SRC_BUCKET_TYPE* si = srcStart; si != srcEnd; ++si) {
                    // start a new x bucket each time through the loop
                    X_OFFSET_TYPE       xLeftInThisSrcBucket =   1.0;

                    //Assert (ti < trgEnd); // not quite cuz of floatpoint roudnd error but should do nearlyequals assert here
                    if (ti >= trgEnd) {
                        break;  //hack!!!
                    }

                    while (xLeftInThisSrcBucket > 0) {
                        // Divide the si value across possibly multiple fractional ti (target) bins!

                        X_OFFSET_TYPE   amount2AdvanceX =   min (xLeftInThisSrcBucket, xLeftInThisTrgBucket);
                        *ti += (*si) * amount2AdvanceX;

                        xLeftInThisSrcBucket -= amount2AdvanceX;
                        xLeftInThisTrgBucket -= amount2AdvanceX;

                        if (xLeftInThisTrgBucket <= 0) {
                            ++ti;
                            xLeftInThisTrgBucket = srcBucketsPerTrgBucket;
                        }
                    }

                }


            }


        }
    }
}
#endif  /*_Stroika_Foundation_Math_ReBin_inl_*/
