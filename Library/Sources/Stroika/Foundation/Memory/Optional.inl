/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_inl_
#define _Stroika_Foundation_Memory_Optional_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Containers/Adapters/Adder.h"
#include "../Debug/Assertions.h"
#include "../Execution/Common.h"
#include "../Execution/Throw.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     ************************ OptionalFromNullable **********************************
     ********************************************************************************
     */
    template <typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, typename T, typename SFINAE_SAFE_CONVERTIBLE>
    inline optional<T> OptionalFromNullable (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* from)
    {
        return from == nullptr ? nullopt : optional<T>{static_cast<T> (*from)};
    }

    /*
     ********************************************************************************
     ******************************** AccumulateIf **********************************
     ********************************************************************************
     */
    template <typename T, typename CONVERTIBLE_TO_T, typename OP, enable_if_t<is_convertible_v<CONVERTIBLE_TO_T, T> and is_convertible_v<OP, function<T (T, T)>>>*>
    void AccumulateIf (optional<T>* lhsOptionalValue, const optional<CONVERTIBLE_TO_T>& rhsOptionalValue, const OP& op)
    {
        if (lhsOptionalValue->has_value ()) {
            if (rhsOptionalValue.has_value ()) {
                *lhsOptionalValue = op (**lhsOptionalValue, static_cast<T> (*rhsOptionalValue));
            }
        }
        else if (rhsOptionalValue.has_value ()) {
            *lhsOptionalValue = static_cast<T> (*rhsOptionalValue);
        }
    }
    template <typename T, typename OP, enable_if_t<is_convertible_v<OP, function<T (T, T)>>>*>
    void AccumulateIf (optional<T>* lhsOptionalValue, const T& rhsValue, const OP& op)
    {
        if (lhsOptionalValue->has_value ()) {
            *lhsOptionalValue = op (**lhsOptionalValue, rhsValue);
        }
        else {
            *lhsOptionalValue = rhsValue;
        }
    }
    template <typename T, template <typename> typename CONTAINER, enable_if_t<is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>>*>
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const optional<T>& rhsOptionalValue)
    {
        if (rhsOptionalValue.has_value ()) {
            if (not lhsOptionalValue->has_value ()) {
                *lhsOptionalValue = CONTAINER<T>{};
            }
            Containers::Adapters::Adder<CONTAINER<T>>::Add (&**lhsOptionalValue, *rhsOptionalValue);
        }
    }
    template <typename T, template <typename> typename CONTAINER, enable_if_t<is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>>*>
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const T& rhsValue)
    {
        if (not lhsOptionalValue->has_value ()) {
            *lhsOptionalValue = CONTAINER<T>{};
        }
        Containers::Adapters::Adder<CONTAINER<T>>::Add (&**lhsOptionalValue, rhsValue);
    }

    /*
     ********************************************************************************
     ************************************ CopyToIf **********************************
     ********************************************************************************
     */
    template <typename T, typename CONVERTABLE_TO_TYPE>
    inline void CopyToIf (const optional<T>& copyFromIfHasValue, CONVERTABLE_TO_TYPE* to)
    {
        if (copyFromIfHasValue) {
            // explicit cast to silence compiler warnigns - use of CopyToIf() is fairly explicit about doing the needed conversions and
            // offers no other direct way to silence the warnings
            *to = static_cast<CONVERTABLE_TO_TYPE> (*copyFromIfHasValue);
        }
    }
    template <typename T, typename CONVERTABLE_TO_OPTIONAL_OF_TYPE>
    inline void CopyToIf (const optional<T>& copyFromIfHasValue, optional<CONVERTABLE_TO_OPTIONAL_OF_TYPE>* to)
    {
        if (copyFromIfHasValue) {
            // explicit cast to silence compiler warnigns - use of CopyToIf() is fairly explicit about doing the needed conversions and
            // offers no other direct way to silence the warnings
            *to = static_cast<CONVERTABLE_TO_OPTIONAL_OF_TYPE> (*copyFromIfHasValue);
        }
    }

    /*
     ********************************************************************************
     ******************************* ValueOrDefault *********************************
     ********************************************************************************
     */
    template <typename T>
    inline T ValueOrDefault (const optional<T>& o, T defaultValue)
    {
        return o.has_value () ? *o : defaultValue;
    }

    /*
     ********************************************************************************
     ******************************** OptionalValue *********************************
     ********************************************************************************
     */
    template <typename T>
    inline optional<T> OptionalValue (const optional<T>& l, const optional<T>& r)
    {
        return l.has_value () ? l : r;
    }

    /*
     ********************************************************************************
     ************************************ operator< *********************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS, typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
    inline bool operator< (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T& rhs)
    {
        return lhs.Compare (rhs) < 0;
    }

    /*
     ********************************************************************************
     ************************************ operator+ *********************************
     ********************************************************************************
     */
    template <typename T>
    optional<T> operator+ (const optional<T>& lhs, const optional<T>& rhs)
    {
        optional<T> result{lhs};
        AccumulateIf (&result, rhs);
        return result;
    }

    /*
     ********************************************************************************
     ************************************ operator- *********************************
     ********************************************************************************
     */
    template <typename T>
    optional<T> operator- (const optional<T>& lhs, const optional<T>& rhs)
    {
        optional<T> result{lhs};
        AccumulateIf (&result, rhs, minus{});
        return result;
    }

    /*
     ********************************************************************************
     ************************************ operator* *********************************
     ********************************************************************************
     */
    template <typename T>
    optional<T> operator* (const optional<T>& lhs, const optional<T>& rhs)
    {
        optional<T> result{lhs};
        AccumulateIf (&result, rhs, multiplies{});
        return result;
    }

    /*
     ********************************************************************************
     ************************************ operator/ *********************************
     ********************************************************************************
     */
    template <typename T>
    optional<T> operator/ (const optional<T>& lhs, const optional<T>& rhs)
    {
        optional<T> result{lhs};
        AccumulateIf (&result, rhs, divides{});
        return result;
    }

}
#endif /*_Stroika_Foundation_Memory_Optional_inl_*/
