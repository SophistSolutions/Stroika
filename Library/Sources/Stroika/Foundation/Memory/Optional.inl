/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
    template <typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, constructible_from<RHS_CONVERTIBLE_TO_OPTIONAL_OF_T> T>
    constexpr optional<T> OptionalFromNullable (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* from)
    {
        return from == nullptr ? nullopt : optional<T>{static_cast<T> (*from)};
    }

    /*
     ********************************************************************************
     ******************************** AccumulateIf **********************************
     ********************************************************************************
     */
    template <typename T, convertible_to<T> CONVERTIBLE_TO_T, convertible_to<function<T (T, T)>> OP>
    void AccumulateIf (optional<T>* lhsOptionalValue, const optional<CONVERTIBLE_TO_T>& rhsOptionalValue, const OP& op)
    {
        RequireNotNull (lhsOptionalValue);
        if (lhsOptionalValue->has_value ()) {
            if (rhsOptionalValue.has_value ()) {
                *lhsOptionalValue = op (**lhsOptionalValue, static_cast<T> (*rhsOptionalValue));
            }
        }
        else if (rhsOptionalValue.has_value ()) {
            *lhsOptionalValue = static_cast<T> (*rhsOptionalValue);
        }
    }
    template <typename T, convertible_to<function<T (T, T)>> OP>
    inline void AccumulateIf (optional<T>* lhsOptionalValue, const T& rhsValue, const OP& op)
    {
        RequireNotNull (lhsOptionalValue);
        if (lhsOptionalValue->has_value ()) {
            *lhsOptionalValue = op (**lhsOptionalValue, rhsValue);
        }
        else {
            *lhsOptionalValue = rhsValue;
        }
    }
    template <typename T, template <typename> typename CONTAINER>
        requires (is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>)
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const optional<T>& rhsOptionalValue)
    {
        RequireNotNull (lhsOptionalValue);
        if (rhsOptionalValue.has_value ()) {
            if (not lhsOptionalValue->has_value ()) {
                *lhsOptionalValue = CONTAINER<T>{};
            }
            Containers::Adapters::Adder<CONTAINER<T>>::Add (&**lhsOptionalValue, *rhsOptionalValue);
        }
    }
    template <typename T, template <typename> typename CONTAINER>
        requires (is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>)
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const T& rhsValue)
    {
        RequireNotNull (lhsOptionalValue);
        if (not lhsOptionalValue->has_value ()) {
            *lhsOptionalValue = CONTAINER<T>{};
        }
        Containers::Adapters::Adder<CONTAINER<T>>::Add (&**lhsOptionalValue, rhsValue);
    }
    template <typename T, convertible_to<T> CONVERTIBLE_TO_T, convertible_to<function<T (T, T)>> OP>
    inline optional<T> AccumulateIf (const optional<T>& lhsOptionalValue, const optional<CONVERTIBLE_TO_T>& rhsOptionalValue, const OP& op)
    {
        optional<T> result{lhsOptionalValue};
        AccumulateIf (&result, rhsOptionalValue, op);
        return result;
    }
    template <typename T, convertible_to<function<T (T, T)>> OP>
    inline optional<T> AccumulateIf (const optional<T>& lhsOptionalValue, const T& rhsValue, const OP& op)
    {
        optional<T> result{lhsOptionalValue};
        AccumulateIf (&result, rhsValue, op);
        return result;
    }
    template <typename T, template <typename> typename CONTAINER>
        requires (is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>)
    optional<CONTAINER<T>> AccumulateIf (const optional<CONTAINER<T>>& lhsOptionalValue, const optional<T>& rhsOptionalValue)
    {
        optional<T> result{lhsOptionalValue};
        AccumulateIf (&result, rhsOptionalValue);
        return result;
    }
    template <typename T, template <typename> typename CONTAINER>
        requires (is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>)
    optional<CONTAINER<T>> AccumulateIf (const optional<CONTAINER<T>>& lhsOptionalValue, const T& rhsValue)
    {
        optional<T> result{lhsOptionalValue};
        AccumulateIf (&result, rhsValue);
        return result;
    }

    /*
     ********************************************************************************
     ************************************ CopyToIf **********************************
     ********************************************************************************
     */
    template <typename T, typename CONVERTABLE_TO_TYPE>
    inline void CopyToIf (CONVERTABLE_TO_TYPE* to, const optional<T>& copyFromIfHasValue)
    {
        if (copyFromIfHasValue) {
            // explicit cast to silence compiler warnings - use of CopyToIf() is fairly explicit about doing the needed conversions and
            // offers no other direct way to silence the warnings
            *to = static_cast<CONVERTABLE_TO_TYPE> (*copyFromIfHasValue);
        }
    }
    template <typename T, typename CONVERTABLE_TO_OPTIONAL_OF_TYPE>
    inline void CopyToIf (optional<CONVERTABLE_TO_OPTIONAL_OF_TYPE>* to, const optional<T>& copyFromIfHasValue)
    {
        if (copyFromIfHasValue) {
            // explicit cast to silence compiler warnings - use of CopyToIf() is fairly explicit about doing the needed conversions and
            // offers no other direct way to silence the warnings
            *to = static_cast<CONVERTABLE_TO_OPTIONAL_OF_TYPE> (*copyFromIfHasValue);
        }
    }

    /*
     ********************************************************************************
     ******************************** NullCoalescing ********************************
     ********************************************************************************
     */
    template <typename T>
    inline optional<T> NullCoalesce (const optional<T>& l, const optional<T>& r)
    {
        return l.has_value () ? l : r;
    }
    template <typename T>
    inline T NullCoalesce (const optional<T>& l, const T& r)
    {
        return l.has_value () ? *l : r;
    }

    /*
     ********************************************************************************
     ************************************ ValueOf ***********************************
     ********************************************************************************
     */
    template <typename T>
    constexpr const T& ValueOf (const optional<T>& t)
    {
        Require (t);
        return *t;
    }

    /*
     ********************************************************************************
     ******************************** ValueOfOrThrow ********************************
     ********************************************************************************
     */
    template <typename EXCEPT, typename T>
    inline const T& ValueOfOrThrow (const optional<T>& t, const EXCEPT& throwIfNull)
    {
        if (t) [[likely]] {
            return *t;
        }
        Execution::Throw (throwIfNull);
    }

    /*
     ********************************************************************************
     ************************************ operator+ *********************************
     ********************************************************************************
     */
    template <typename T>
    inline optional<T> operator+ (const optional<T>& lhs, const optional<T>& rhs)
    {
        if (lhs and rhs) {
            return *lhs + *rhs;
        }
        return nullopt;
    }
    template <typename T>
    inline optional<T> operator+ (const optional<T>& lhs, const T& rhs)
    {
        if (lhs) {
            return (*lhs) + rhs;
        }
        return nullopt;
    }
    template <typename T>
    inline optional<T> operator+ (const T& lhs, const optional<T>& rhs)
    {
        if (rhs) {
            return lhs + (*rhs);
        }
        return nullopt;
    }

    /*
     ********************************************************************************
     ************************************ operator- *********************************
     ********************************************************************************
     */
    template <typename T>
    inline optional<T> operator- (const optional<T>& lhs, const optional<T>& rhs)
    {
        if (lhs and rhs) {
            return *lhs - *rhs;
        }
        return nullopt;
    }
    template <typename T>
    inline optional<T> operator- (const optional<T>& lhs, const T& rhs)
    {
        if (lhs) {
            return (*lhs) - rhs;
        }
        return nullopt;
    }
    template <typename T>
    inline optional<T> operator- (const T& lhs, const optional<T>& rhs)
    {
        if (rhs) {
            return lhs - (*rhs);
        }
        return nullopt;
    }

    /*
     ********************************************************************************
     ************************************ operator* *********************************
     ********************************************************************************
     */
    template <typename T>
    inline optional<T> operator* (const optional<T>& lhs, const optional<T>& rhs)
    {
        if (lhs and rhs) {
            return (*lhs) * (*rhs);
        }
        return nullopt;
    }
    template <typename T>
    inline optional<T> operator* (const optional<T>& lhs, const T& rhs)
    {
        if (lhs) {
            return (*lhs) * rhs;
        }
        return nullopt;
    }
    template <typename T>
    inline optional<T> operator* (const T& lhs, const optional<T>& rhs)
    {
        if (rhs) {
            return lhs * (*rhs);
        }
        return nullopt;
    }

    /*
     ********************************************************************************
     ************************************ operator/ *********************************
     ********************************************************************************
     */
    template <typename T>
    inline optional<T> operator/ (const optional<T>& lhs, const optional<T>& rhs)
    {
        if (lhs) {
            return (*lhs) / rhs;
        }
    }
    template <typename T>
    inline optional<T> operator/ (const optional<T>& lhs, const T& rhs)
    {
        if (lhs) {
            return (*lhs) / rhs;
        }
        return nullopt;
    }
    template <typename T>
    inline optional<T> operator/ (const T& lhs, const optional<T>& rhs)
    {
        if (rhs) {
            return lhs / (*rhs);
        }
        return nullopt;
    }

}
#endif /*_Stroika_Foundation_Memory_Optional_inl_*/
