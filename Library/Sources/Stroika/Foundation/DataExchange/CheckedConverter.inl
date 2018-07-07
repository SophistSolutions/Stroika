/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_CheckedConverter_inl_
#define _Stroika_Foundation_DataExchange_CheckedConverter_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <type_traits>

#include "../Characters/String_Constant.h"
#include "../Execution/Exceptions.h"
#include "../Math/Common.h"

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {

            /*
             ********************************************************************************
             ***************************** DataExchange::Private_ ***************************
             ********************************************************************************
             */
            namespace Private_ {
                template <typename T>
                inline enable_if_t<not std::is_floating_point<T>::value, T> CheckedConverter_Range_Helper_Pinner_ (T t, T /*lower*/, T /*upper*/)
                {
                    return t;
                }
                template <typename T>
                inline enable_if_t<std::is_floating_point<T>::value, T> CheckedConverter_Range_Helper_Pinner_ (T t, T lower, T upper)
                {
                    return Math::PinToSpecialPoint (Math::PinToSpecialPoint (t, lower), upper);
                }
            }

            /*
             ********************************************************************************
             ******************** DataExchange::CheckedConverter_Range **********************
             ********************************************************************************
             */
            template <typename RANGE_TYPE>
            RANGE_TYPE CheckedConverter_Range (const typename RANGE_TYPE::value_type& s, const typename RANGE_TYPE::value_type& e)
            {
                using Characters::String_Constant;
                typename RANGE_TYPE::value_type useS = Private_::CheckedConverter_Range_Helper_Pinner_ (s, RANGE_TYPE::TraitsType::kLowerBound, RANGE_TYPE::TraitsType::kUpperBound);
                typename RANGE_TYPE::value_type useE = Private_::CheckedConverter_Range_Helper_Pinner_ (e, RANGE_TYPE::TraitsType::kLowerBound, RANGE_TYPE::TraitsType::kUpperBound);
                // Note: these checks MUST use <= and >= and IGNORE openness, because the bounds need not be in the range.
                if (not(RANGE_TYPE::TraitsType::kLowerBound <= useS)) {
                    Execution::Throw (BadFormatException (String_Constant (L"Value < RangeType lower bounds")));
                }
                if (not(useS <= useE)) {
                    Execution::Throw (BadFormatException (String_Constant (L"Range start must be less than end")));
                }
                if (not(useE <= RANGE_TYPE::TraitsType::kUpperBound)) {
                    Execution::Throw (BadFormatException (String_Constant (L"Range end must be less than Range traits end")));
                }
                return RANGE_TYPE (useS, useE);
            }

            /*
             ********************************************************************************
             *************** DataExchange::CheckedConverter_ValueInRange ********************
             ********************************************************************************
             */
            template <typename RANGE_TYPE>
            typename RANGE_TYPE::value_type CheckedConverter_ValueInRange (typename RANGE_TYPE::value_type val, const RANGE_TYPE& range)
            {
                using Characters::String_Constant;
                typename RANGE_TYPE::value_type useVal = Private_::CheckedConverter_Range_Helper_Pinner_ (val, RANGE_TYPE::TraitsType::kLowerBound, RANGE_TYPE::TraitsType::kUpperBound);
                if (not range.Contains (useVal)) {
                    if (useVal <= range.GetLowerBound ()) {
                        Execution::Throw (BadFormatException (String_Constant (L"Value out of range (too low)")));
                    }
                    else {
                        Assert (useVal >= range.GetUpperBound ());
                        Execution::Throw (BadFormatException (String_Constant (L"Value out of range (exceeds max)")));
                    }
                }
                return useVal;
            }
        }
    }
}
#endif /*_Stroika_Foundation_DataExchange_CheckedConverter_inl_*/
