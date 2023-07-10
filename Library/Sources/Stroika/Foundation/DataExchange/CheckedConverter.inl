/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_CheckedConverter_inl_
#define _Stroika_Foundation_DataExchange_CheckedConverter_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <type_traits>

#include "../Execution/Throw.h"
#include "../Math/Common.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ***************************** DataExchange::Private_ ***************************
     ********************************************************************************
     */
    namespace Private_ {
        template <typename T>
        inline T CheckedConverter_Range_Helper_Pinner_ (T t, T lower, T upper)
        {
            if constexpr (is_floating_point_v<T>) {
                return Math::PinToSpecialPoint (Math::PinToSpecialPoint (t, lower), upper);
            }
            else {
                // @todo review this code - where called - not sure why not pin in range too here?
                return t;
            }
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
        typename RANGE_TYPE::value_type useS =
            Private_::CheckedConverter_Range_Helper_Pinner_ (s, RANGE_TYPE::TraitsType::kLowerBound, RANGE_TYPE::TraitsType::kUpperBound);
        typename RANGE_TYPE::value_type useE =
            Private_::CheckedConverter_Range_Helper_Pinner_ (e, RANGE_TYPE::TraitsType::kLowerBound, RANGE_TYPE::TraitsType::kUpperBound);
        // Note: these checks MUST use <= and >= and IGNORE openness, because the bounds need not be in the range.
        if (not(RANGE_TYPE::TraitsType::kLowerBound <= useS)) {
            Execution::Throw (BadFormatException{"Value less than RangeType lower bounds"sv});
        }
        if (not(useS <= useE)) {
            Execution::Throw (BadFormatException{"Range start must be less than or equal to end"sv});
        }
        if (not(useE <= RANGE_TYPE::TraitsType::kUpperBound)) {
            Execution::Throw (BadFormatException{"Range end must be less than or equal to Range traits end"sv});
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
        typename RANGE_TYPE::value_type useVal =
            Private_::CheckedConverter_Range_Helper_Pinner_ (val, RANGE_TYPE::TraitsType::kLowerBound, RANGE_TYPE::TraitsType::kUpperBound);
        if (not range.Contains (useVal)) {
            if (useVal <= range.GetLowerBound ()) {
                Execution::Throw (BadFormatException{"Value out of range (too low)"sv});
            }
            else {
                Assert (useVal >= range.GetUpperBound ());
                Execution::Throw (BadFormatException{"Value out of range (exceeds max)"sv});
            }
        }
        return useVal;
    }

}

#endif /*_Stroika_Foundation_DataExchange_CheckedConverter_inl_*/
