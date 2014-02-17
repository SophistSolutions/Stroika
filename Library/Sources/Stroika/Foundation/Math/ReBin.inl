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
             ********************************** Math::Overlaps ******************************
             ********************************************************************************
             */
            /*

            two iterators - one marking (start/end of target buckets), and one marking current src bucket.

            iteratoe over outer buckets. move contents to new bucket. And adjust new iteraotrs. When they overlpap and must
                adnvace - proprtionally add to bucket, adnvace and add rest to next target bucket.

            */
            template    <typename SRC_BUCKET_TYPE, typename TRG_BUCKET_TYPE, typename X_OFFSET_TYPE>
            void    ReBin (
                const SRC_BUCKET_TYPE* srcStart, const SRC_BUCKET_TYPE* srcEnd,
                TRG_BUCKET_TYPE* trgStart, TRG_BUCKET_TYPE* trgEnd
            )
            {
                size_t  nSrcBuckets =   srcEnd - srcStart;
                size_t  nTrgBuckets =   trgEnd - trgStart;

                // zero target bins
                for (TRG_BUCKET_TYPE i = trgStart; i != trgEnd; ++i) {
                    *i = 0;
                }

                X_OFFSET_TYPE       trgBucketsPerSrcBucket = static_cast<X_OFFSET_TYPE> (nTrgBuckets) / static_cast<X_OFFSET_TYPE> (nSrcBuckets);
                TRG_BUCKET_TYPE     ti  =   trgStart;
                X_OFFSET_TYPE       fracLeftInThisTrgBucket =   trgBucketsPerSrcBucket;

                for (const SRC_BUCKET_TYPE* si = srcStart; si != srcEnd; ++si) {
                    //Assert (ti < trgEnd); // not quite cuz of floatpoint roudnd error but should do nearlyequals assert here
                    if (ti >= trgEnd) {
                        break;  //hack!!!
                    }

                    // Divide the si value across possibly multiple fractional ti (target) bins!
                    if (fracLeftInThisTrgBucket < 1) {
                        *ti += (*si) * fracLeftInThisTrgBucket;
                        SRC_BUCKET_TYPE     rollOverFromLastBucket  =   (*si) * (1 - fracLeftInThisTrgBucket);
                        fracLeftInThisTrgBucket = 0;
                        ++ti;
                        //Assert (ti < trgEnd); // not quite cuz of floatpoint roudnd error but should do nearlyequals assert here
                        if (ti < trgEnd) {
                            *ti += rollOverFromLastBucket;
                        }
                    }
                    else {
                        *ti += (*si);
                        fracLeftInThisTrgBucket -= 1;
                        ++ti;
                    }
                }


            }


        }
    }
}
#endif  /*_Stroika_Foundation_Math_ReBin_inl_*/
