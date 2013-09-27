/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_CheckedConverter_inl_
#define _Stroika_Foundation_DataExchangeFormat_CheckedConverter_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#include    "../Execution/Exceptions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            template    <typename   RANGE_TYPE>
            RANGE_TYPE  CheckedConverter_Range (const typename RANGE_TYPE::ElementType& s, const typename RANGE_TYPE::ElementType& e)
            {
                if (not (RANGE_TYPE::kMin <= s)) {
                    Execution::DoThrow (BadFormatException ());
                }
                if (not (s <= e)) {
                    Execution::DoThrow (BadFormatException ());
                }
                if (not (e <= RANGE_TYPE::kMax)) {
                    Execution::DoThrow (BadFormatException ());
                }
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_CheckedConverter_inl_*/
