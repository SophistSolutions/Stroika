/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_inl_
#define _Stroika_Foundation_Characters_ToString_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <exception>
#include <filesystem>
#include <functional>
#include <ranges>
#include <typeindex>
#include <typeinfo>
#include <wchar.h>

#include "../Configuration/Concepts.h"
#include "../Configuration/Enumeration.h"
#include "FloatConversion.h"
#include "Format.h"
#include "StringBuilder.h"

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

    template <>
    String ToString (const std::filesystem::path& t);

    namespace Private_ {

        template <typename T>
        using has_ToStringMethod_t_ = decltype (static_cast<Characters::String> (declval<const T&> ().ToString ()));
        template <typename T>
        using has_pair_t_ = decltype (declval<const T&> ().first, declval<const T&> ().second);
        template <typename T>
        using has_KeyValuePair_t_ = decltype (declval<const T&> ().fKey, declval<const T&> ().fValue);
        template <typename T>
        using has_CountedValue_t_ = decltype (declval<const T&> ().fValue, declval<const T&> ().fCount);

        /**
         *  \brief checks if the given type has a .ToString () const method returning a string
         */
        template <typename T>
        constexpr inline bool has_ToStringMethod_v = Configuration::is_detected_v<Private_::has_ToStringMethod_t_, T>;

        /**
         *  \brief this given type appears to be a 'pair' of some sort
         */
        template <typename T>
        constexpr inline bool has_pair_v = Configuration::is_detected_v<Private_::has_pair_t_, T>;

        /**
         *  \brief this given type appears to be a 'KeyValuePair' of some sort
         */
        template <typename T>
        constexpr inline bool has_KeyValuePair_v = Configuration::is_detected_v<Private_::has_KeyValuePair_t_, T>;

        /**
         *  \brief this given type appears to be a 'CountedValue' of some sort
         */
        template <typename T>
        constexpr inline bool has_CountedValue_v = Configuration::is_detected_v<Private_::has_CountedValue_t_, T>;

        template <typename T>
        inline String ToString_ (const T& t)
            requires (has_ToStringMethod_v<T>)
        {
            return t.ToString ();
        }
        template <ranges::range T>
        String ToString_ (const T& t)
            requires (not has_ToStringMethod_v<T> and not is_convertible_v<T, String>)
        {
            StringBuilder sb;
            sb << "[";
            bool didFirst{false};
            for (const auto& i : t) {
                if (didFirst) {
                    sb << ", "_k;
                }
                else {
                    sb << " "_k;
                }
                sb << ToString (i);
                didFirst = true;
            }
            if (didFirst) {
                sb << " "_k;
            }
            sb << "]"_k;
            return sb.str ();
        }
        template <typename T>
        inline String ToString_ (const T& t)
            requires (is_convertible_v<T, String>)
        {
            constexpr size_t kMaxLen2Display_{100}; // no idea what a good value here will be or if we should provide ability to override. I suppose
                                                    // users can template-specialize ToString(const String&)???
            return "'"sv + static_cast<String> (t).LimitLength (kMaxLen2Display_) + "'"sv;
        }

        String ToString_ex_ (const exception& t);

        template <typename T>
        inline String ToString_ (const T& t)
            requires (is_convertible_v<T, const exception&>)
        {
            return ToString_ex_ (t);
        }
        template <typename T>
        inline String ToString_ ([[maybe_unused]] const T& t)
            requires (is_convertible_v<T, tuple<>>)
        {
            return "{}"sv;
        }
        template <typename T1>
        String ToString_ (const tuple<T1>& t)
        {
            StringBuilder sb;
            sb << "{";
            sb << ToString (t);
            sb << "}";
            return sb.str ();
        }
        template <typename T1, typename T2>
        String ToString_ (const tuple<T1, T2>& t)
        {
            StringBuilder sb;
            sb << "{";
            sb << ToString (get<0> (t)) + ", " + ToString (get<1> (t));
            sb << "}";
            return sb.str ();
        }
        template <typename T1, typename T2, typename T3>
        String ToString_ (const tuple<T1, T2, T3>& t)
        {
            StringBuilder sb;
            sb << "{";
            sb << ToString (get<0> (t)) + ", " + ToString (get<1> (t)) + ", " + ToString (get<2> (t));
            sb << "}";
            return sb.str ();
        }
        template <typename T1, typename T2, typename T3, typename T4>
        String ToString_ (const tuple<T1, T2, T3>& t)
        {
            StringBuilder sb;
            sb << "{";
            sb << ToString (get<0> (t)) + ", " + ToString (get<1> (t)) + ", " + ToString (get<2> (t)) + ", " + ToString (get<3> (t));
            sb << "}";
            return sb.str ();
        }
        template <typename T>
        String ToString_ (const T& t)
            requires (has_pair_v<T>)
        {
            StringBuilder sb;
            sb << "{";
            sb << ToString (t.first) << ": " << ToString (t.second);
            sb << "}";
            return sb.str ();
        }
        template <typename T>
        String ToString_ (const T& t)
            requires (has_KeyValuePair_v<T>)
        {
            StringBuilder sb;
            sb << "{";
            sb << ToString (t.fKey) << ": " << ToString (t.fValue);
            sb << "}";
            return sb.str ();
        }
        template <typename T>
        String ToString_ (const T& t)
            requires (has_CountedValue_v<T>)
        {
            StringBuilder sb;
            sb << "{";
            sb << "'" << ToString (t.fValue) << "': " << ToString (t.fCount);
            sb << "}";
            return sb.str ();
        }
        template <typename T>
        inline String ToString_ (const T& t)
            requires (is_enum_v<T>)
        {
            // SHOULD MAYBE only do if can detect is-defined Configuration::DefaultNames<T>, but right now not easy, and
            // not a problem: just don't call this, or replace it with a specific specialization of ToString
            return Configuration::DefaultNames<T>{}.GetName (t);
        }
        template <typename T>
        inline String ToString_ (const T& t)
            requires (is_floating_point_v<T>)
        {
            return FloatConversion::ToString (t);
        }
        template <typename T>
        inline String ToString_ (const shared_ptr<T>& pt)
        {
            return (pt == nullptr) ? L"nullptr"sv : Format (L"%p", pt.get ());
        }
        template <typename T>
        inline String ToString_ (const unique_ptr<T>& pt)
        {
            return (pt == nullptr) ? L"nullptr"sv : Format (L"%p", pt.get ());
        }
        template <typename T>
        inline String ToString_ (const optional<T>& o)
        {
            return o.has_value () ? Characters::ToString (*o) : "[missing]"sv;
        }
        template <typename FUNCTION_SIGNATURE>
        inline String ToString_ (const function<FUNCTION_SIGNATURE>& f)
        {
            return Format (L"%p", f.template target<remove_cvref_t<FUNCTION_SIGNATURE>> ());
        }

    }

    template <>
    inline String ToString (const signed char& t)
    {
        return ToString (t, std::ios_base::dec);
    }
    template <>
    inline String ToString (const short int& t)
    {
        return ToString (t, std::ios_base::dec);
    }
    template <>
    inline String ToString (const int& t)
    {
        return ToString (t, std::ios_base::dec);
    }
    template <>
    inline String ToString (const long int& t)
    {
        return ToString (t, std::ios_base::dec);
    }
    template <>
    inline String ToString (const long long int& t)
    {
        return ToString (t, std::ios_base::dec);
    }
    template <>
    inline String ToString (const unsigned char& t)
    {
        return ToString (t, std::ios_base::hex);
    }
    template <>
    inline String ToString (const unsigned short& t)
    {
        return ToString (t, std::ios_base::dec);
    }
    template <>
    inline String ToString (const unsigned int& t)
    {
        return ToString (t, std::ios_base::dec);
    }
    template <>
    inline String ToString (const unsigned long& t)
    {
        return ToString (t, std::ios_base::dec);
    }
    template <>
    inline String ToString (const unsigned long long& t)
    {
        return ToString (t, std::ios_base::dec);
    }

    template <>
    inline String ToString (const std::byte& t)
    {
        return ToString (static_cast<unsigned char> (t), std::ios_base::hex);
    }

    template <>
    inline String ToString (const std::filesystem::path& t)
    {
        return ToString (t.wstring ()); // wrap in 'ToString' for surrounding quotes
    }

    template <typename T>
    inline String ToString (const T& t)
    {
        return Private_::ToString_ (t);
    }

}

#endif /*_Stroika_Foundation_Characters_ToString_inl_*/
