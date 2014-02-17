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
                 *      iterate over outer buckets. Move contents to new bucket. And adjust new iterators. When they overlpap and must
                 *      advance - proprtionally add to bucket, adnvace and add rest to next target bucket.
                 */
                size_t  nSrcBuckets =   srcEnd - srcStart;
                size_t  nTrgBuckets =   trgEnd - trgStart;

                // zero target bins
                for (TRG_BUCKET_TYPE* i = trgStart; i != trgEnd; ++i) {
                    *i = 0;
                }

                X_OFFSET_TYPE       trgBucketsPerSrcBucket = static_cast<X_OFFSET_TYPE> (nTrgBuckets) / static_cast<X_OFFSET_TYPE> (nSrcBuckets);
                TRG_BUCKET_TYPE*    ti  =   trgStart;
                X_OFFSET_TYPE       fracLeftInThisTrgBucket =   trgBucketsPerSrcBucket;

                /*
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
                    //Assert (ti < trgEnd); // not quite cuz of floatpoint roudnd error but should do nearlyequals assert here
                    if (ti >= trgEnd) {
                        break;  //hack!!!
                    }

                    // Divide the si value across possibly multiple fractional ti (target) bins!
                    if (fracLeftInThisTrgBucket < 1) {
                        *ti += (*si) * fracLeftInThisTrgBucket;
                        TRG_BUCKET_TYPE     rollOverFromLastBucket  =   (*si) * (1 - fracLeftInThisTrgBucket);  // even though data is all from src bucket use TRG_BUCKET_TYPE since thats where its going and could be fractional??? (not clear)
                        fracLeftInThisTrgBucket = 0;
                        ++ti;
                        fracLeftInThisTrgBucket = trgBucketsPerSrcBucket;
                        //Assert (ti < trgEnd); // not quite cuz of floatpoint roudnd error but should do nearlyequals assert here
                        if (ti < trgEnd) {
                            *ti += rollOverFromLastBucket;
                        }
                    }
                    else {
                        *ti += (*si);
                        fracLeftInThisTrgBucket -= 1;
                        ++ti;
                        if (fracLeftInThisTrgBucket == 0) {
                            fracLeftInThisTrgBucket = trgBucketsPerSrcBucket;
                        }
                    }
                }


            }


        }
    }
}
#endif  /*_Stroika_Foundation_Math_ReBin_inl_*/
