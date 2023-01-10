/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Versions_h_
#define _Stroika_Foundation_IO_Network_HTTP_Versions_h_ 1

#include "../../../StroikaPreComp.h"

#include <map>
#include <string>

#include "../../../Configuration/Common.h"

/*
 * TODO:
 *      @todo       When we have a good C++ 'static string' class - maybe use that here.
 *                  Maybe ONLY can do once we have compiler constexpr support?
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    // standard HTTP Versions one might want to access/retrieve
    namespace Versions {

        constexpr string_view kOnePointZero = "1.0"sv;
        constexpr string_view kOnePointOne  = "1.1"sv;
        constexpr string_view kTwoPointZero = "2.0"sv;
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Versions.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Versions_h_*/
