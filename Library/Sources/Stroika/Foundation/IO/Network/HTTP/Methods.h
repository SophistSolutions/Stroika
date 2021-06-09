/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Methods_h_
#define _Stroika_Foundation_IO_Network_HTTP_Methods_h_ 1

#include "../../../StroikaPreComp.h"

#include <string>

#include "../../../Characters/RegularExpression.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     *  Standard HTTP METHODS
     *
     *  \note https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html#sec5.1.1
     *        "The method is case-sensitive"
     *
     *  \note https://tools.ietf.org/html/rfc5789 (for PATCH)
     */
    namespace Methods {
        constexpr wstring_view kGet     = L"GET"sv;
        constexpr wstring_view kPatch   = L"PATCH"sv;
        constexpr wstring_view kPut     = L"PUT"sv;
        constexpr wstring_view kPost    = L"POST"sv;
        constexpr wstring_view kDelete  = L"DELETE"sv;
        constexpr wstring_view kOptions = L"OPTIONS"sv;
        constexpr wstring_view kHead    = L"HEAD"sv;
    }

    namespace MethodsRegEx {
        using namespace Characters;
        // DIDNT DEFINE kANY nor kOptions because typically a mistake for users to be using them... Better to allow the system to handle autoamtically
        //inline const RegularExpression kANY       = L".*"_RegEx; /*RegularExpression::kAny*/
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
        // HACK - DEF IN REQUEST.CPP
        extern const RegularExpression kGet;
        extern const RegularExpression kPut;
        extern const RegularExpression kPatch;
        extern const RegularExpression kPost;
        extern const RegularExpression kPostOrPut;
        extern const RegularExpression kDelete;
#else
        inline const RegularExpression kGet       = L"GET"_RegEx;
        inline const RegularExpression kPut       = L"PUT"_RegEx;
        inline const RegularExpression kPatch     = L"PATCH"_RegEx;
        inline const RegularExpression kPost      = L"POST"_RegEx;
        inline const RegularExpression kPostOrPut = L"PUT|POST"_RegEx;
        inline const RegularExpression kDelete    = L"DELETE"_RegEx;
        //inline const RegularExpression kOptions   = L"OPTIONS"_RegEx;
#endif
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Methods.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Methods_h_*/
