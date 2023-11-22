/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_inl_
#define _Stroika_Foundation_Characters_ToString_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <chrono>
#include <exception>
#include <filesystem>
#include <functional>
#include <ranges>
#include <thread>
#include <typeindex>
#include <typeinfo>
#include <wchar.h>

#include "../Configuration/Concepts.h"
#include "../Configuration/Enumeration.h"
#include "FloatConversion.h"
#include "Format.h"
#include "StringBuilder.h"

namespace Stroika::Foundation::Characters {

    namespace Private_ {

        /**
         *  \brief checks if the given type has a .ToString () const method returning a string
         */
        template <typename T>
        concept has_ToStringMethod_v = requires (T t) {
            {
                t.ToString ()
            } -> convertible_to<Characters::String>;
        };
        /**
         *  \brief this given type appears to be a 'pair' of some sort
         */
        template <typename T>
        concept has_pair_v = requires (T t) {
            t.first;
            t.second;
        };
        /**
         *  \brief this given type appears to be a 'KeyValuePair' of some sort
         */
        template <typename T>
        concept has_KeyValuePair_v = requires (T t) {
            t.fKey;
            t.fValue;
        };
        /**
         *  \brief this given type appears to be a 'CountedValue' of some sort
         */
        template <typename T>
        concept has_CountedValue_v = requires (T t) {
            t.fValue;
            t.fCount;
        };

        template <typename T>
        inline String num2Str_ (T t, ios_base::fmtflags flags)
        {
            static_assert (sizeof (t) <= sizeof (int));
            wchar_t buf[1024];
            switch (flags) {
                case ios_base::oct:
                    (void)::swprintf (buf, Memory::NEltsOf (buf), L"%o", t);
                    break;
                case ios_base::dec:
                    (void)::swprintf (buf, Memory::NEltsOf (buf), L"%d", t);
                    break;
                case ios_base::hex:
                    (void)::swprintf (buf, Memory::NEltsOf (buf), L"0x%x", t);
                    break;
                default:
                    AssertNotReached (); // @todo support octal
            }
            return buf;
        }
        template <typename T>
        inline String num2Strl_ (T t, ios_base::fmtflags flags)
        {
            wchar_t buf[1024];
            static_assert (sizeof (t) == sizeof (long int));
            switch (flags) {
                case ios_base::oct:
                    (void)::swprintf (buf, Memory::NEltsOf (buf), L"%lo", t);
                    break;
                case ios_base::dec:
                    (void)::swprintf (buf, Memory::NEltsOf (buf), L"%ld", t);
                    break;
                case ios_base::hex:
                    (void)::swprintf (buf, Memory::NEltsOf (buf), L"0x%lx", t);
                    break;
                default:
                    AssertNotReached (); // @todo support octal
            }
            return buf;
        }
        template <typename T>
        inline String num2Strll_ (T t, ios_base::fmtflags flags)
        {
            wchar_t buf[1024];
            static_assert (sizeof (t) == sizeof (long long int));
            switch (flags) {
                case ios_base::oct:
                    (void)::swprintf (buf, Memory::NEltsOf (buf), L"%llo", t);
                    break;
                case ios_base::dec:
                    (void)::swprintf (buf, Memory::NEltsOf (buf), L"%lld", t);
                    break;
                case ios_base::hex:
                    (void)::swprintf (buf, Memory::NEltsOf (buf), L"0x%llx", t);
                    break;
                default:
                    AssertNotReached (); // @todo support octal
            }
            return buf;
        }

    }

    /**
     *  Collect all the default ToString() implementations, templates, overloads etc, all in one namespace.
     * 
     *  Users of the Stroika library may specialize Characters::ToString(), but (???) probably should not add overloads ot the
     *  ToStringDefaults namespace --LGP 2023-11-21.
     */
    namespace ToStringDefaults {

        // IN CPP FILE
        String ToString (const exception_ptr& t);
        String ToString (const exception& t);
        String ToString (const type_info& t);
        String ToString (const type_index& t);
        String ToString (const thread::id& t);
        String ToString (bool t);

        template <Private_::has_ToStringMethod_v T>
        inline String ToString (const T& t)
        {
            return t.ToString ();
        }
        template <ranges::range T>
        String ToString (const T& t)
            requires (not Private_::has_ToStringMethod_v<T> and not is_convertible_v<T, String>)
        {
            StringBuilder sb;
            sb << "["sv;
            bool didFirst{false};
            for (const auto& i : t) {
                if (didFirst) {
                    sb << ", "_k;
                }
                else {
                    sb << " "_k;
                }
                sb << Characters::ToString (i);
                didFirst = true;
            }
            if (didFirst) {
                sb << " "_k;
            }
            sb << "]"_k;
            return sb.str ();
        }
        template <typename T>
        inline String ToString (const T& t)
            requires (is_convertible_v<T, String>)
        {
            constexpr size_t kMaxLen2Display_{100}; // no idea what a good value here will be or if we should provide ability to override. I suppose
                                                    // users can template-specialize ToString(const String&)???
            return "'"sv + static_cast<String> (t).LimitLength (kMaxLen2Display_) + "'"sv;
        }
        template <typename T>
        inline String ToString ([[maybe_unused]] const T& t)
            requires (is_convertible_v<T, tuple<>>)
        {
            return "{}"sv;
        }
        template <typename T1>
        String ToString (const tuple<T1>& t)
        {
            StringBuilder sb;
            sb << "{"sv << Characters::ToString (t) << "}"sv;
            return sb.str ();
        }
        template <typename T1, typename T2>
        String ToString (const tuple<T1, T2>& t)
        {
            StringBuilder sb;
            sb << "{"sv << Characters::ToString (get<0> (t)) << ", "sv << Characters::ToString (get<1> (t)) << "}"sv;
            return sb.str ();
        }
        template <typename T1, typename T2, typename T3>
        String ToString (const tuple<T1, T2, T3>& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (get<0> (t)) << ", "sv << Characters::ToString (get<1> (t)) << ", "sv << Characters::ToString (get<2> (t));
            sb << "}"sv;
            return sb.str ();
        }
        template <typename T1, typename T2, typename T3, typename T4>
        String ToString (const tuple<T1, T2, T3>& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (get<0> (t)) << ", "sv << Characters::ToString (get<1> (t))
               << ", "sv + Characters::ToString (get<2> (t)) << ", "sv << Characters::ToString (get<3> (t));
            sb << "}"sv;
            return sb.str ();
        }
        template <typename T>
        String ToString (const T& t)
            requires (Private_::has_pair_v<T>)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (t.first) << ": "sv << Characters::ToString (t.second);
            sb << "}"sv;
            return sb.str ();
        }
        template <Private_::has_KeyValuePair_v T>
        String ToString (const T& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (t.fKey) << ": "sv << Characters::ToString (t.fValue);
            sb << "}"sv;
            return sb.str ();
        }
        template <Private_::has_CountedValue_v T>
        String ToString (const T& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << "'" << Characters::ToString (t.fValue) << "': "sv << Characters::ToString (t.fCount);
            sb << "}"sv;
            return sb.str ();
        }
        template <typename T>
        inline String ToString (const T& t)
            requires (is_enum_v<T>)
        {
            if constexpr (not Configuration::IBoundedEnum<T>) {
                return Characters::ToString (underlying_type_t<T> (t));
            }
            else if constexpr (Configuration::DefaultNames<T>{}.size () == 0) {
                // emit as number if no EnumNames<> declared
                return Characters::ToString (underlying_type_t<T> (t));
            }
            else {
                return Configuration::DefaultNames<T>{}.GetName (t);
            }
        }
        template <floating_point T>
        inline String ToString (T t)
        {
            return FloatConversion::ToString (t);
        }
        template <typename T>
        inline String ToString (const shared_ptr<T>& pt)
        {
            return (pt == nullptr) ? L"nullptr"sv : Format (L"%p", pt.get ());
        }
        template <typename T>
        inline String ToString (const unique_ptr<T>& pt)
        {
            return (pt == nullptr) ? L"nullptr"sv : Format (L"%p", pt.get ());
        }
        template <typename T>
        inline String ToString (const optional<T>& o)
        {
            return o.has_value () ? Characters::ToString (*o) : "[missing]"sv;
        }
        template <typename FUNCTION_SIGNATURE>
        inline String ToString (const function<FUNCTION_SIGNATURE>& f)
        {
            return Format (L"%p", f.template target<remove_cvref_t<FUNCTION_SIGNATURE>> ());
        }
        inline String ToString (const chrono::duration<double>& t)
        {
            return Characters::ToString (t.count ()) + " seconds"sv;
        }
        template <typename CLOCK_T>
        inline String ToString (const chrono::time_point<CLOCK_T, chrono::duration<double>>& t)
        {
            return Characters::ToString (t.time_since_epoch ().count ()) + " seconds"sv;
        }
        template <integral T>
        inline String ToString (T t, ios_base::fmtflags flags)
        {
            using namespace Private_;
            if constexpr (sizeof (T) < sizeof (long)) {
                return num2Str_ (t, flags);
            }
            else if constexpr (sizeof (T) == sizeof (long)) {
                return num2Strl_ (t, flags);
            }
            else if constexpr (sizeof (T) == sizeof (long long int)) {
                return num2Strll_ (t, flags);
            }
        }
        template <signed_integral T>
        inline String ToString (T t)
        {
            return Characters::ToString (t, ios_base::dec);
        }
        template <unsigned_integral T>
        inline String ToString (T t)
        {
            // no overwhelmingly good reason todo it this way, but this matches what we had in Stroika 2.1, and its reasonable...
            if constexpr (sizeof (T) == 1) {
                return Characters::ToString (t, ios_base::hex);
            }
            else {
                return Characters::ToString (t, ios_base::dec);
            }
        }
        inline String ToString (byte t)
        {
            return Characters::ToString (static_cast<unsigned char> (t), ios_base::hex);
        }
        inline String ToString (const filesystem::path& t)
        {
            return Characters::ToString (t.wstring ()); // wrap in 'ToString' for surrounding quotes
        }

    }

    /*
     ********************************************************************************
     *************************** Characters::ToString *******************************
     ********************************************************************************
     */
    template <typename T, typename... ARGS>
    inline String ToString (T&& t, ARGS... args)
    {
        return ToStringDefaults::ToString (forward<T> (t), forward<ARGS> (args)...);
    }
    template <integral T>
    inline String ToString (T t, ios_base::fmtflags flags)
    {
        return ToStringDefaults::ToString (t, flags);
    }

    //DEPRECATED
    namespace Private_ {
        template <typename T>
        using has_ToString_t = decltype (static_cast<Characters::String> (declval<T&> ().ToString ()));
    }
    /*
     *  \brief Return true iff Characters::ToString (T) is well defined.
     */
    template <typename T>
    [[deprecated ("Since Stroika v3.0d5 use IToString")]] constexpr inline bool has_ToString_v =
        Configuration::is_detected_v<Private_::has_ToString_t, T>;

}

#endif /*_Stroika_Foundation_Characters_ToString_inl_*/
