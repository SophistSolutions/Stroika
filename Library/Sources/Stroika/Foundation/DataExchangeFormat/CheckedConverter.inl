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
namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            template    <typename   RANGE_TYPE>
            RANGE_TYPE  CheckedConverter_Range (typename const RANGE_TYPE::ElementType& s, typename const RANGE_TYPE::ElementType& e)
            {
                if (not (RANGE_TYPE::kMin <= s)) {
                    Exception::DoThrow (BadFormatException ());
                }
                if (not (s <= e)) {
                    Exception::DoThrow (BadFormatException ());
                }
                if (not (e <= RANGE_TYPE::kMax)) {
                    Exception::DoThrow (BadFormatException ());
                }
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_CheckedConverter_inl_*/
