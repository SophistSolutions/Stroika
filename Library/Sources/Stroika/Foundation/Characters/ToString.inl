/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_inl_
#define _Stroika_Foundation_Characters_ToString_inl_ 1

#include <exception>
#include <typeindex>
#include <typeinfo>
#include <wchar.h>

#include "../Configuration/Concepts.h"
#include "../Configuration/Enumeration.h"
#include "FloatConversion.h"
#include "StringBuilder.h"
#include "String_Constant.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ********************************* ToString *************************************
     ********************************************************************************
     */
	template <>
	String ToString (const exception_ptr& t);
	template <>
	String ToString (const type_info& t);
	template <>
	String ToString (const type_index& t);

	String ToString (const char* t);

	/*
	 * From section from section 3.9.1 of http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf
	 *      There are five standard signed integer types : signed char, short int, int,
	 *      long int, and long long int. In this list, each type provides at least as much
	 *      storage as those preceding it in the list.
	 *      For each of the standard signed integer types, there exists a corresponding (but different)
	 *      standard unsigned integer type: unsigned char, unsigned short int, unsigned int, unsigned long int,
	 *      and unsigned long long int, each of which occupies the same amount of storage and has the
	 *      same alignment requirements.
	 */
	template <>
	String ToString (const bool& t);
	template <>
	String ToString (const signed char& t);
	template <>
	String ToString (const short int& t);
	template <>
	String ToString (const int& t);
	template <>
	String ToString (const long int& t);
	template <>
	String ToString (const long long int& t);
	template <>
	String ToString (const unsigned char& t);
	template <>
	String ToString (const unsigned short& t);
	template <>
	String ToString (const unsigned int& t);
	template <>
	String ToString (const unsigned long& t);
	template <>
	String ToString (const unsigned long long& t);

	namespace Private_ {

        STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (ToString, (x.ToString ()));
        STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (beginenditerable, (x.begin () != x.end ()));
        STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (pair, (x.first, x.second));
        STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (KeyValuePair, (x.fKey, x.fValue));
        STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (CountedValue, (x.fValue, x.fCount));
        STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (name, (x.name (), x.name ()));

        template <typename T>
        inline String ToString_ (const T& t, enable_if_t<has_ToString<T>::value>* = 0)
        {
            return t.ToString ();
        }
        template <typename T>
        String ToString_ (const T& t, enable_if_t<has_beginenditerable<T>::value and not has_ToString<T>::value and not is_convertible_v<T, String>>* = 0)
        {
            StringBuilder sb;
            sb << L"[";
            bool didFirst{false};
            for (auto i : t) {
                if (didFirst) {
                    sb << L", ";
                }
                else {
                    sb << L" ";
                }
                sb << ToString (i);
                didFirst = true;
            }
            if (didFirst) {
                sb << L" ";
            }
            sb << L"]";
            return sb.str ();
        }
        template <typename T>
        inline String ToString_ (const T& t, enable_if_t<is_convertible_v<T, String>>* = 0)
        {
            return L"'" + static_cast<String> (t) + L"'";
        }
        template <typename T>
        inline String ToString_ (const T& t, enable_if_t<is_convertible_v<T, const exception&>>* = 0)
        {
            //saying Exception: first produces 'Exception: HTTP exception: status 404 (URL not found)}' - redundant. Not sure about all cases, but try this way.
            //return String_Constant {L"Exception: " } + String::FromNarrowSDKString (t.what ()) + String_Constant {L"}" };
            return String::FromNarrowSDKString (t.what ());
        }
        inline String ToString_ ([[maybe_unused]] const tuple<>& t)
        {
            StringBuilder sb;
            sb << L"{";
            sb << L"}";
            return sb.str ();
        }
        template <typename T1>
        String ToString_ (const tuple<T1>& t)
        {
            StringBuilder sb;
            sb << L"{";
            sb << ToString (t);
            sb << L"}";
            return sb.str ();
        }
        template <typename T1, typename T2>
        String ToString_ (const tuple<T1, T2>& t)
        {
            StringBuilder sb;
            sb << L"{";
            sb << ToString (get<0> (t)) + L", " + ToString (get<1> (t));
            sb << L"}";
            return sb.str ();
        }
        template <typename T1, typename T2, typename T3>
        String ToString_ (const tuple<T1, T2, T3>& t)
        {
            StringBuilder sb;
            sb << L"{";
            sb << ToString (get<0> (t)) + L", " + ToString (get<1> (t)) + L", " + ToString (get<2> (t));
            sb << L"}";
            return sb.str ();
        }
        template <typename T1, typename T2, typename T3, typename T4>
        String ToString_ (const tuple<T1, T2, T3>& t)
        {
            StringBuilder sb;
            sb << L"{";
            sb << ToString (get<0> (t)) + L", " + ToString (get<1> (t)) + L", " + ToString (get<2> (t)) + L", " + ToString (get<3> (t));
            sb << L"}";
            return sb.str ();
        }
        template <typename T>
        String ToString_ (const T& t, enable_if_t<has_pair<T>::value>* = 0)
        {
            StringBuilder sb;
            sb << L"{";
            sb << L"'" << ToString (t.first) << L"': " << ToString (t.second);
            sb << L"}";
            return sb.str ();
        }
        template <typename T>
        String ToString_ (const T& t, enable_if_t<has_KeyValuePair<T>::value>* = 0)
        {
            StringBuilder sb;
            sb << L"{";
            sb << L"'" << ToString (t.fKey) << L"': " << ToString (t.fValue);
            sb << L"}";
            return sb.str ();
        }
        template <typename T>
        String ToString_ (const T& t, enable_if_t<has_CountedValue<T>::value>* = 0)
        {
            StringBuilder sb;
            sb << L"{";
            sb << L"'" << ToString (t.fValue) << L"': " << ToString (t.fCount);
            sb << L"}";
            return sb.str ();
        }
        template <typename T>
        inline String ToString_ (const T& t, enable_if_t<is_enum_v<T>>* = 0)
        {
            // SHOULD MAYBE only do if can detect is-defined Configuration::DefaultNames<T>, but right now not easy, and
            // not a problem: just don't call this, or replace it with a specific specialization of ToString
            return Configuration::DefaultNames<T>::k.GetName (t);
        }
        template <typename T, size_t SZ>
        String ToString_array_ (const T (&arr)[SZ])
        {
            StringBuilder sb;
            sb << L"[";
            for (size_t i = 0; i < SZ; ++i) {
                sb << L" " << ToString (arr[i]);
                if (i + 1 < SZ) {
                    sb << L",";
                }
                else {
                    sb << L" ";
                }
            }
            sb << L"]";
            return sb.str ();
        }
        template <typename T>
        inline String ToString_ (const T& t, enable_if_t<is_array_v<T> and not is_convertible_v<T, String>>* = 0)
        {
            return ToString_array_ (t);
        }
        template <typename T>
        inline String ToString_ (const T& t, enable_if_t<is_floating_point_v<T>>* = 0)
        {
            return Float2String (t);
        }
        template <typename T>
        inline String ToString_ (const shared_ptr<T>& pt)
        {
            return (pt == nullptr) ? L"nullptr" : ToString (*pt);
        }
        template <typename T>
        inline String ToString_ (const optional<T>& o)
        {
            return o.has_value () ? Characters::ToString (*o) : L"[missing]";
        }
    }

    template <typename T>
    inline String ToString (const T& t)
    {
        return Private_::ToString_ (t);
    }

}

#endif /*_Stroika_Foundation_Characters_ToString_inl_*/
