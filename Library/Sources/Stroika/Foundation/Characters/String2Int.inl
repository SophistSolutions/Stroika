/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <charconv>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

namespace Stroika::Foundation::Characters {

    namespace Private_ {
        template <integral T, IUNICODECodePoint CHAR_T>
        T String2IntSpan_ (span<const CHAR_T> s);
    }

    template <typename T = int>
    [[deprecated ("Since Stroika v3.0d1, use span{} overload")]] T String2Int (const wchar_t* start, const wchar_t* end)
    {
        return Private_::String2IntSpan_<T> (span<const wchar_t>{start, end});
    }

    namespace Private_ {
        unsigned long long int String2UInt_ (const String& s);
        long long int          String2Int_ (const String& s);
        DISABLE_COMPILER_MSC_WARNING_START (4018)
        template <integral T>
        T String2IntOrUInt_ (const String& s)
        {
            if constexpr (numeric_limits<T>::is_signed) {
                long long int l = String2Int_ (s);
                if (l <= numeric_limits<T>::min ()) {
                    return numeric_limits<T>::min ();
                }
                if (l >= numeric_limits<T>::max ()) {
                    return numeric_limits<T>::max ();
                }
                return static_cast<T> (l);
            }
            else {
                unsigned long long int l = String2UInt_ (s);
                if (l >= numeric_limits<T>::max ()) {
                    return numeric_limits<T>::max ();
                }
                return static_cast<T> (l);
            }
        }
        DISABLE_COMPILER_MSC_WARNING_END (4018)
    }

    /*
     ********************************************************************************
     *********************************** String2Int *********************************
     ********************************************************************************
     */
    namespace Private_ {
        // String2Int's implementation, so the deprecated overloads need not call one another
        template <integral T, IUNICODECodePoint CHAR_T>
        T String2IntSpan_ (span<const CHAR_T> s)
        {
            Require ((String{s} == String{s}.Trim ()));
            if (s.empty ()) {
                return 0;
            }
            // @todo this can be further simplfied and optimized (ESPECIALLY the size=1 case)
            if constexpr (sizeof (CHAR_T) == sizeof (wchar_t)) {
                /*
             *  Most of the time we can do this very efficiently, because there are just ascii characters.
             *  Else, fallback on older algorithm that understands full unicode character set.
             */
                Memory::StackBuffer<char> asciiS;
                if (Character::AsASCIIQuietly (s, &asciiS)) {
                    Stroika_ATTRIBUTE_INDETERMINATE T r;
                    auto                              b = asciiS.begin ();
                    auto                              e = asciiS.end ();
                    if (b != e and *b == '+') {
                        ++b; // "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)" from https://en.cppreference.com/w/cpp/utility/from_chars
                    }
                    auto [ptr, ec] = from_chars (b, e, r);
                    if (ec == errc::result_out_of_range) [[unlikely]] {
                        return *b == '-' ? numeric_limits<T>::min () : numeric_limits<T>::max ();
                    }
                    // if error or trailing crap - return 0
                    T result = (ec == std::errc{} and ptr == e) ? r : 0; // a weird default, but what the algorithm advertises and for its not sure there is better?
                    Ensure (result == Private_::String2IntOrUInt_<T> (String{s})); // test backward compat with old algorithm --LGP 2021-11-08
                    return result;
                }
                else {
                    return Private_::String2IntOrUInt_<T> (String{s});
                }
            }
            else {
                String                       ss{s};
                Memory::StackBuffer<wchar_t> ignored;
                return String2IntSpan_<T> (ss.GetData<wchar_t> (&ignored));
            }
        }
    }
    template <integral T, IUNICODECodePoint CHAR_T>
    inline T String2Int (span<const CHAR_T> s)
    {
        return Private_::String2IntSpan_<T> (s);
    }
    template <integral T, IConvertibleToString STRINGISH_ARG>
    inline T String2Int (STRINGISH_ARG&& s)
    {
        using DecayedStringishArg = remove_cvref_t<STRINGISH_ARG>;
        if constexpr (same_as<DecayedStringishArg, const char*> or same_as<DecayedStringishArg, const char8_t*> or
                      same_as<DecayedStringishArg, const char16_t*> or same_as<DecayedStringishArg, const char32_t*> or
                      same_as<DecayedStringishArg, const wchar_t*>) {
            return Private_::String2IntSpan_<T> (span{s, CString::Length (s)});
        }
        else if constexpr (same_as<DecayedStringishArg, String>) {
            // @todo PERFORMANCE TWEEK - peek or just use ascii
            Memory::StackBuffer<wchar_t> ignored;
            auto                         sp = s.template GetData<wchar_t> (&ignored);
            return Private_::String2IntSpan_<T> (sp);
        }
        else {
            String                       ss{forward<STRINGISH_ARG> (s)};
            Memory::StackBuffer<wchar_t> ignored;
            return Private_::String2IntSpan_<T> (ss.GetData<wchar_t> (&ignored));
        }
    }

}
