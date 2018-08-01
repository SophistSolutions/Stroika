/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
namespace Stroika {
    namespace Foundation {
        namespace Characters {

            /*
             ********************************************************************************
             ********************************* ToString *************************************
             ********************************************************************************
             */
            namespace Private_ {

                STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (ToString, (x.ToString ()));
                STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (beginenditerable, (x.begin () != x.end ()));
                STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (pair, (x.first, x.second));
                STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (KeyValuePair, (x.fKey, x.fValue));
                STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (CountedValue, (x.fValue, x.fCount));
                STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (name, (x.name (), x.name ()));

                template <typename T>
                inline String ToString_ (const T& t, typename enable_if<has_ToString<T>::value>::type* = 0)
                {
                    return t.ToString ();
                }
                template <typename T>
                String ToString_ (const T& t, typename enable_if<has_beginenditerable<T>::value and not has_ToString<T>::value and not is_convertible<T, String>::value>::type* = 0)
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
                inline String ToString_ (const T& t, typename enable_if<is_convertible<T, String>::value>::type* = 0)
                {
                    return static_cast<String> (t);
                }
                template <typename T>
                inline String ToString_ (const T& t, typename enable_if<is_convertible<T, const std::exception&>::value>::type* = 0)
                {
                    //saying Exception: first produces 'Exception: HTTP exception: status 404 (URL not found)}' - redundant. Not sure about all cases, but try this way.
                    //return String_Constant {L"Exception: " } + String::FromNarrowSDKString (t.what ()) + String_Constant {L"}" };
                    return String::FromNarrowSDKString (t.what ());
                }
                template <typename T>
                String ToString_ (const T& t, typename enable_if<has_pair<T>::value>::type* = 0)
                {
                    StringBuilder sb;
                    sb << L"{";
                    sb << L"'" << ToString (t.first) << L"': " << ToString (t.second);
                    sb << L"}";
                    return sb.str ();
                }
                template <typename T>
                String ToString_ (const T& t, typename enable_if<has_KeyValuePair<T>::value>::type* = 0)
                {
                    StringBuilder sb;
                    sb << L"{";
                    sb << L"'" << ToString (t.fKey) << L"': " << ToString (t.fValue);
                    sb << L"}";
                    return sb.str ();
                }
                template <typename T>
                String ToString_ (const T& t, typename enable_if<has_CountedValue<T>::value>::type* = 0)
                {
                    StringBuilder sb;
                    sb << L"{";
                    sb << L"'" << ToString (t.fValue) << L"': " << ToString (t.fCount);
                    sb << L"}";
                    return sb.str ();
                }
                template <typename T>
                inline String ToString_ (const T& t, typename enable_if<is_enum<T>::value>::type* = 0)
                {
                    // SHOULD MAYBE only do if can detect is-defined Configuration::DefaultNames<T>, but right now not easy, and
                    // not a problem: just dont call this, or replace it with a specific specialization of ToString
                    return Configuration::DefaultNames<T>::k.GetName (t);
                }
                template <typename T, size_t SZ>
                inline String ToString_array_ (const T (&arr)[SZ])
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
                inline String ToString_ (const T& t, typename enable_if<is_array<T>::value and not is_convertible<T, String>::value>::type* = 0)
                {
                    return ToString_array_ (t);
                }
                template <typename T>
                inline String ToString_ (const T& t, typename enable_if<is_floating_point<T>::value>::type* = 0)
                {
                    return Float2String (t);
                }
                template <typename T>
                inline String ToString_ (const shared_ptr<T>& pt)
                {
                    return (pt == nullptr) ? L"nullptr" : ToString (*pt);
                }
#if qCompilerAndStdLib_Supports_stdoptional || qCompilerAndStdLib_Supports_stdexperimentaloptional
                template <typename T>
                inline String ToString_ (const optional<T>& o)
                {
                    return o.has_value () ? Characters::ToString (*o) : L"[missing]";
                }
#endif
            }

            template <typename T>
            inline String ToString (const T& t)
            {
                return Private_::ToString_ (t);
            }

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
        }
    }
}
#endif /*_Stroika_Foundation_Characters_ToString_inl_*/
