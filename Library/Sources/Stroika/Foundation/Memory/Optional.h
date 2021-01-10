/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_h_
#define _Stroika_Foundation_Memory_Optional_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <optional>
#include <shared_mutex>

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/Empty.h"
#include "../Containers/Adapters/Adder.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Execution/NullMutex.h"
#include "BlockAllocated.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 *      NOTE TO SUGGEST TO C++ standards - 
 *          Things I miss most about my Optional versus std::optional
 *              >   Value () - what they call value_or - should take T{} as default argument. About 25% of teh time
 *                  thats what I want, and its much more clear/terse.
 *
 *              >   Accumulate method, and operator +=, operator-= etc overloads calling Accumulate(). Much simpler
 *                  and more elegant code with those methods.
 */

namespace Stroika::Foundation::Characters {
    class String;
    template <typename T>
    String ToString (const T&);
}

namespace Stroika::Foundation::Memory {

    using std::byte;

    /**
     *  EXPERIEMNTAL - 2015-04-24
     *  \brief  AccumulateIf () add in the rhs argument value to lhs optional, but if both were missing leave 'lhs'
     *          as still missing, and if only RHS available, assign it to the left.
     *
     *  \par Example Usage
     *      \code
     *          optional<int>   accumulator;
     *          optional<int>   SomeFunctionToGetOptionalValue();
     *          if (accumulator or (tmp = SomeFunctionToGetOptionalValue())) {
     *              accumulator = accumulator.Value () + tmp;
     *          }
     *      \endcode
     *      VERSUS
     *      \code
     *          accumulator.AccumulateIf (SomeFunctionToGetOptionalValue ());
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          optional<Sequence<InternetAddress>>>   addresses;
     *          Memory::AccumulateIf (&addresses, IO::Network::InternetAddress{connection.GET ().GetDataTextInputStream ().ReadAll ().Trim ()});
     *      \endcode
     *
     *
     *      \note   ITS CONFUSING direction of if-test for this versus CopyToIf
     *
     *      \note
     *          typical OP arguments would be:
     *              std::plus{} **the default**
     *              std::minus{}
     *              std::multiplies{}
     *              std::divides{}
     */
    template <typename T, typename CONVERTIBLE_TO_T, typename OP = plus<T>, enable_if_t<is_convertible_v<CONVERTIBLE_TO_T, T> and is_convertible_v<OP, function<T (T, T)>>>* = nullptr>
    void AccumulateIf (optional<T>* lhsOptionalValue, const optional<CONVERTIBLE_TO_T>& rhsOptionalValue, const OP& op = OP{});
    template <typename T, typename OP = plus<T>, enable_if_t<is_convertible_v<OP, function<T (T, T)>>>* = nullptr>
    void AccumulateIf (optional<T>* lhsOptionalValue, const T& rhsValue, const OP& op = OP{});
    template <typename T, template <typename> typename CONTAINER, enable_if_t<is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>>* = nullptr>
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const optional<T>& rhsOptionalValue);
    template <typename T, template <typename> typename CONTAINER, enable_if_t<is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>>* = nullptr>
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const T& rhsValue);

    /**
     *  Assign the value held by this optional if one is present to destination argument (pointer). Assigns from left to right.
     *
     *  The point of this to to faciltate a common idiom, where you want to maintain an existing value unless you
     *  get an update. This function is ANALAGOUS to
     *      if (o.has_value()) {
     *          destArgVal = *o;
     *      }
     *
     *  but can be done in a single line.
     *
     *  \par Example Usage
     *      \code
     *          int curValue = 3;
     *          optional<long>  oVal = someMap.Lookup (KEY_VALUE);
     *          oVal.CopyToIf (&curValue);
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          optional<int> curValue;
     *          optional<long>  oVal = someMap.Lookup (KEY_VALUE);
     *          oVal.CopyToIf (&curValue);      // curValue retains its value from before CopyToIf if oVal was missing
     *      \endcode
     *
     *  @see Value
     *
     *  \note - the overload with the second arg optional<CONVERTABLE_TO_OPTIONAL_OF_TYPE> is just to silence an msvc compiler warning.
     */
    template <typename T, typename CONVERTABLE_TO_TYPE>
    void CopyToIf (const optional<T>& copyFromIfHasValue, CONVERTABLE_TO_TYPE* to);
    template <typename T, typename CONVERTABLE_TO_OPTIONAL_OF_TYPE>
    void CopyToIf (const optional<T>& copyFromIfHasValue, optional<CONVERTABLE_TO_OPTIONAL_OF_TYPE>* to);

    /**
     *  Always safe to call. If has_value returns it, else returns argument 'default'. Like value_or () but has default value for default value.
     *
     *  \note This is handy because there is no default argument for std::optional<>::value_or () - there should be (like this).
     */
    template <typename T>
    T ValueOrDefault (const optional<T>& o, T defaultValue = T{});

    /**
     * \brief return one of *this, or o, with first preference for which is engaged, and second preference for left-to-right.
     *
     *  So Equivilent to this->has_value ()? *this : o;
     *
     *  This is VERY similar to ValueOrDefault () - except that the default maybe optional, and this therefore returns an Optional<T>
     *
     *  @see Value ()
     */
    template <typename T>
    optional<T> OptionalValue (const optional<T>& l, const optional<T>& r);

    /**
     *  'Constructor' taking const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* is to allow easier interoperability
     *  with code that uses null-pointers to mean 'is-missing': nullptr means missing, and if non null,
     *  derefrence and copy.
     *
     *  \par Example Usage
     *      \code
     *      float*  d1  =   nullptr;
     *      double* d2  =   nullptr;
     *      Assert (not Optional<double>::OptionalFromNullable (d1).has_value ());
     *      Assert (not Optional<double>::OptionalFromNullable (d2).has_value ());
     *      \endcode
     *
     *  \note   I tried making this an Optional<T> constructor overload, but it lead to dangerous confusion with things like
     *          URL url = URL (L"dyn:/StyleSheet.css?ThemeName=Cupertino", URL::eStroikaPre20a50BackCompatMode);
     *          VerifyTestResult (url.GetScheme () == L"dyn");
     *          // wchar_t* overload is optional gets STRING with value "d";
     */
    template <typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, typename T = RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, typename SFINAE_SAFE_CONVERTIBLE = enable_if_t<Configuration::is_explicitly_convertible<RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, T>::value>>
    optional<T> OptionalFromNullable (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* from);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator+= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T>
    optional<T> operator+ (const optional<T>& lhs, const optional<T>& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator-= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T>
    optional<T> operator- (const optional<T>& lhs, const optional<T>& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator*= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T>
    optional<T> operator* (const optional<T>& lhs, const optional<T>& rhs);

    /**
     *  Simple overloaded operator which calls @Optional<T>::operator/= (const Optional<T>& rhs)
     *
     *  \note this uses AccumulateIf(), so if both sides missing, result is missing, but if only one side is
     *        empty, the other side defaults to T{}.
     */
    template <typename T>
    optional<T> operator/ (const optional<T>& lhs, const optional<T>& rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Optional.inl"

#endif /*_Stroika_Foundation_Memory_Optional_h_*/
