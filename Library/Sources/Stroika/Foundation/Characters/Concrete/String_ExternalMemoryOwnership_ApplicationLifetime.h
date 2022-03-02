/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_h_
#define _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_h_ 1

#include "../../StroikaPreComp.h"

#include <string_view>

#include "../String.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   Consider adding platform-specific code to detect if the argument is in fact read-only memory
 *              or stack memory, with debug asserts - as a debugging aid to help prevent accidental mis-use.
 */

namespace Stroika::Foundation::Characters::Concrete {

    /**
     *  \brief String_ExternalMemoryOwnership_ApplicationLifetime allow creation of String objects with fewer memory allocations, and more efficient storage, but only in constrained situations
     *
     *      String_ExternalMemoryOwnership_ApplicationLifetime is a subtype of string you can
     * use to construct a String object, so long as the memory pointed to in the argument has a
     *      o   FULL APPLICATION LIFETIME,
     *      o   the member referenced never changes - is READONLY.
     *
     *      String_ExternalMemoryOwnership_ApplicationLifetime will NOT change the memory
     * referenced in the CTOR.
     *
     *      Strings constructed with this String_ExternalMemoryOwnership_ApplicationLifetime
     * maybe treated like normal strings - passed anywhere, and even modified via the String APIs.
     *
     *  \par Example:
     *      \code
     *          String  tmp1    =   L"FRED";
     *          String  tmp2    =   String{L"FRED"};
     *          String  tmp3    =   String_ExternalMemoryOwnership_ApplicationLifetime{L"FRED"};
     *          String  tmp4    =   L"FRED"sv;
     *
     *          extern String saved;
     *          inline  String  F(String x)         { saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
     *          F(tmp1);
     *          F(tmp2);
     *          F(tmp3);
     *          F(tmp4);
     *      \endcode
     *
     *      These ALL do essentially the same thing, and are all equally safe. The 'tmp3' implementation
     * maybe slightly more efficent, but all are equally safe.
     *
     *  \em WARNING - BE VERY CAREFUL - be sure arguments have application lifetime.
     * 
     *  \req argument string must be nul-terminated (but CAN contain additional embedded nul characters)
     */
    class String_ExternalMemoryOwnership_ApplicationLifetime : public String {
    private:
        using inherited = String;

    public:
        /**
         *  The constructor requires an application lifetime NUL-terminated array of characters - such as one
         *  created with L"sample" (but allows embedded NUL-characters).
         *
         *      \req SIZE >= 1
         *      \req cString[SIZE-1] == '\0'
         *
         *      \req *end == '\0' and start + ::wcslen (start) <= end;
         */
        template <size_t SIZE>
        explicit String_ExternalMemoryOwnership_ApplicationLifetime (const wchar_t (&cString)[SIZE]);
        String_ExternalMemoryOwnership_ApplicationLifetime (const wchar_t* start, const wchar_t* end);
        String_ExternalMemoryOwnership_ApplicationLifetime (const basic_string_view<wchar_t>& str);
        String_ExternalMemoryOwnership_ApplicationLifetime (String_ExternalMemoryOwnership_ApplicationLifetime&& s) noexcept      = default;
        String_ExternalMemoryOwnership_ApplicationLifetime (const String_ExternalMemoryOwnership_ApplicationLifetime& s) noexcept = default;

    public:
        nonvirtual String_ExternalMemoryOwnership_ApplicationLifetime& operator= (String_ExternalMemoryOwnership_ApplicationLifetime&& s) noexcept = default;
        nonvirtual String_ExternalMemoryOwnership_ApplicationLifetime& operator= (const String_ExternalMemoryOwnership_ApplicationLifetime& s) = default;

    private:
        class MyRep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "String_ExternalMemoryOwnership_ApplicationLifetime.inl"

#endif /*_Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_h_*/
