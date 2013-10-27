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

#include    <type_traits>

#include    "../Math/Common.h"
#include    "../Execution/Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            /*
             ********************************************************************************
             *********************** DataExchangeFormat::Private_ ***************************
             ********************************************************************************
             */
            namespace Private_ {
                template    <typename T>
                inline  typename std::enable_if < !std::is_floating_point<T>::value, T >::type CheckedConverter_Range_Helper_Pinner_ (T t, T lower, T upper)
                {
                    return t;
                }
                template    <typename T>
                inline  typename std::enable_if<std::is_floating_point<T>::value, T>::type  CheckedConverter_Range_Helper_Pinner_ (T t, T lower, T upper)
                {
                    return Math::PinToSpecialPoint (PinToSpecialPoint (t, lower), upper);
                }
            }


            /*
             ********************************************************************************
             ************** DataExchangeFormat::CheckedConverter_Range **********************
             ********************************************************************************
             */
            template    <typename   RANGE_TYPE>
            RANGE_TYPE  CheckedConverter_Range (const typename RANGE_TYPE::ElementType& s, const typename RANGE_TYPE::ElementType& e)
            {
                typename    RANGE_TYPE::ElementType useS    =   Private_::CheckedConverter_Range_Helper_Pinner_ (s, RANGE_TYPE::TraitsType::kMin, RANGE_TYPE::TraitsType::kMax);
                typename    RANGE_TYPE::ElementType useE    =   Private_::CheckedConverter_Range_Helper_Pinner_ (e, RANGE_TYPE::TraitsType::kMin, RANGE_TYPE::TraitsType::kMax);
                if (not (RANGE_TYPE::TraitsType::kMin <= useS)) {
                    Execution::DoThrow (BadFormatException ());
                }
                if (not (useS <= useE)) {
                    Execution::DoThrow (BadFormatException ());
                }
                if (not (useE <= RANGE_TYPE::TraitsType::kMax)) {
                    Execution::DoThrow (BadFormatException ());
                }
                return RANGE_TYPE (useS, useE);
            }


            /*
             ********************************************************************************
             ********* DataExchangeFormat::CheckedConverter_ValueInRange ********************
             ********************************************************************************
             */
            template    <typename   RANGE_TYPE>
            typename RANGE_TYPE::ElementType  CheckedConverter_ValueInRange (typename RANGE_TYPE::ElementType val, const RANGE_TYPE& r)
            {
                typename    RANGE_TYPE::ElementType useVal    =   Private_::CheckedConverter_Range_Helper_Pinner_ (val, RANGE_TYPE::TraitsType::kMin, RANGE_TYPE::TraitsType::kMax);
                if (not (RANGE_TYPE::TraitsType::kMin <= useVal)) {
                    Execution::DoThrow (BadFormatException ());
                }
                if (not (useVal <= RANGE_TYPE::TraitsType::kMax)) {
                    Execution::DoThrow (BadFormatException ());
                }
                return useVal;
            }



        }

    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_CheckedConverter_inl_*/
