/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Methods_h_
#define _Stroika_Foundation_IO_Network_HTTP_Methods_h_ 1

#include "../../../StroikaPreComp.h"

#include <string>

#include "../../../Characters/RegularExpression.h"
#include "../../../Configuration/Common.h"

/*
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     *  Standard HTTP METHODS
     *
     *  \note https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html#sec5.1.1
     *        "The method is case-sensitive"
     */
    namespace Methods {
        constexpr wstring_view kGet     = L"GET";
        constexpr wstring_view kPut     = L"PUT";
        constexpr wstring_view kPost    = L"POST";
        constexpr wstring_view kDelete  = L"DELETE";
        constexpr wstring_view kOptions = L"OPTIONS";
    }

    namespace MethodsRegularExpressions {
        extern const Characters::RegularExpression kGet;
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
