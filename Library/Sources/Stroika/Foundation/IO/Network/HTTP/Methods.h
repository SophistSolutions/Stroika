/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Methods_h_
#define _Stroika_Foundation_IO_Network_HTTP_Methods_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <string>

#include "Stroika/Foundation/Characters/RegularExpression.h"

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
        constexpr string_view kGet     = "GET"sv;
        constexpr string_view kPatch   = "PATCH"sv;
        constexpr string_view kPut     = "PUT"sv;
        constexpr string_view kPost    = "POST"sv;
        constexpr string_view kDelete  = "DELETE"sv;
        constexpr string_view kOptions = "OPTIONS"sv;
        constexpr string_view kHead    = "HEAD"sv;
    }

    namespace MethodsRegEx {
        using namespace Characters;
        // DIDNT DEFINE kANY  because typically a mistake for users to be using them... Better to allow the system to handle automatically
        //inline const RegularExpression kANY       = L".*"_RegEx; /*RegularExpression::kAny*/
        inline const RegularExpression kGet       = "GET"_RegEx;
        inline const RegularExpression kPut       = "PUT"_RegEx;
        inline const RegularExpression kPatch     = "PATCH"_RegEx;
        inline const RegularExpression kPost      = "POST"_RegEx;
        inline const RegularExpression kPostOrPut = "PUT|POST"_RegEx;
        inline const RegularExpression kDelete    = "DELETE"_RegEx;
        inline const RegularExpression kOptions   = "OPTIONS"_RegEx; // not typically used - handled automatically by webserver
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Methods.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Methods_h_*/
