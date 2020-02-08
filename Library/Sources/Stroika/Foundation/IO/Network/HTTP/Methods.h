/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Methods_h_
#define _Stroika_Foundation_IO_Network_HTTP_Methods_h_ 1

#include "../../../StroikaPreComp.h"

#include <string>

#include "../../../Characters/RegularExpression.h"

/*
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
    }

    namespace MethodsRegularExpressions {
        extern const Characters::RegularExpression kGet;
        extern const Characters::RegularExpression kPatch;
        extern const Characters::RegularExpression kPut;
        extern const Characters::RegularExpression kPost;
        extern const Characters::RegularExpression kPostOrPut;
        extern const Characters::RegularExpression kDelete;
        extern const Characters::RegularExpression kOptions;
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Methods.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Methods_h_*/
