/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Adapters/Adder.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/Throw.h"

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
     ********************************* NullCoalesce *********************************
     ********************************************************************************
     */
    template <Private_::INullCoalescable OT>
    inline const OT& NullCoalesce (const OT& l, const OT& r)
    {
        if (l) {
            return l;
        }
        return r;
    }
    template <Private_::INullCoalescable OT, convertible_to<const Private_::OptionalType2ValueType<OT>&> DEFAULT_TYPE>
    inline const Private_::OptionalType2ValueType<OT>& NullCoalesce (const OT& l, const DEFAULT_TYPE& r)
    {
        if (l) {
            return *l;
        }
        return r;
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
    template <typename T, typename EXCEPT>
    inline const T& ValueOfOrThrow (const optional<T>& t, const EXCEPT& throwIfNull)
    {
        Execution::ThrowIfNull (t, throwIfNull);
        return *t;
    }

    /*
     ********************************************************************************
     ******************************** OptionallyCopy ********************************
     ********************************************************************************
     */
    template <typename OUT_T, Common::explicitly_convertible_to<OUT_T> IN_T>
    inline optional<OUT_T> OptionallyCopy (const optional<IN_T>& in)
    {
        if (in) {
            return OUT_T{*in};
        }
        return nullopt;
    }
    template <typename OUT_T, typename IN_T, invocable<IN_T> IN_TO_OUT_CONVERTER>
    inline optional<OUT_T> OptionallyCopy (const optional<IN_T>& in, IN_TO_OUT_CONVERTER&& cvt)
        requires (convertible_to<invoke_result_t<IN_TO_OUT_CONVERTER, IN_T>, optional<OUT_T>>)
    {
        if (in) {
            return forward<IN_TO_OUT_CONVERTER> (cvt) (*in);
        }
        return nullopt;
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
