/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Methods_h_
#define _Stroika_Foundation_IO_Network_HTTP_Methods_h_ 1

#include "../../../StroikaPreComp.h"

#include <map>
#include <string>

#include "../../../Configuration/Common.h"

/*
 * TODO:
 *      (o)     consider using std::string_view?
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    // standard HTTP METHODS
    namespace Methods {
        constexpr wchar_t kGet[]     = L"GET";
        constexpr wchar_t kPut[]     = L"PUT";
        constexpr wchar_t kPost[]    = L"POST";
        constexpr wchar_t kDelete[]  = L"DELETE";
        constexpr wchar_t kOptions[] = L"OPTIONS";
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Methods.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Methods_h_*/
