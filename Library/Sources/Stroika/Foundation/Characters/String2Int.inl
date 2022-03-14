/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String2Int_inl_
#define _Stroika_Foundation_Characters_String2Int_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <charconv>

#include "../Configuration/Common.h"
#include "../Memory/StackBuffer.h"

namespace Stroika::Foundation::Characters {

    namespace Private_ {
        unsigned long long int String2UInt_ (const String& s);
        long long int          String2Int_ (const String& s);
        DISABLE_COMPILER_MSC_WARNING_START (4018)
        template <typename T>
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
    template <typename T>
    T String2Int (const wchar_t* start, const wchar_t* end)
    {
        Require ((String{start, end} == String{start, end}.Trim ()));

        /*
             *  Most of the time we can do this very efficiently, because there are just ascii characters.
             *  Else, fallback on older algorithm that understands full unicode character set.
             */
        Memory::StackBuffer<char> asciiS;
        if (String::AsASCIIQuietly (start, end, &asciiS)) {
            T    r; // intentionally uninitialized
            auto b = asciiS.begin ();
            auto e = asciiS.end ();
            if (b != e and *b == '+') {
                ++b; // "the plus sign is not recognized outside of the exponent (only the minus sign is permitted at the beginning)" from https://en.cppreference.com/w/cpp/utility/from_chars
            }
            auto [ptr, ec] = from_chars (b, e, r);
            if (ec == errc::result_out_of_range) [[unlikely]] {
                return *b == '-' ? numeric_limits<T>::min () : numeric_limits<T>::max ();
            }
            // if error or trailing crap - return nan
            T result = (ec == std::errc () and ptr == e) ? r : 0;                   // a wierd default, but what the algorithm advertises and for its not sure there is better?
            Ensure (result == Private_::String2IntOrUInt_<T> (String{start, end})); // test backward compat with old algorithm --LGP 2021-11-08
            return result;
        }
        else {
            return Private_::String2IntOrUInt_<T> (String{start, end});
        }
    }
    template <typename T>
    inline T String2Int (const String& s)
    {
        auto [start, end] = s.GetData<wchar_t> ();
        return String2Int<T> (start, end);
    }

}

#endif /*_Stroika_Foundation_Characters_String2Int_inl_*/
