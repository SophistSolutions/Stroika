/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ********************* InternetMediaTypeNotSupportedException *******************
     ********************************************************************************
     */
    inline InternetMediaTypeNotSupportedException::InternetMediaTypeNotSupportedException ()
        : Execution::Exception<runtime_error>{"Internet Media Type not supported"sv}
    {
    }
    inline InternetMediaTypeNotSupportedException::InternetMediaTypeNotSupportedException (const InternetMediaType& mediaType)
        : Execution::Exception<runtime_error>{Characters::FormatString{"Internet Media Type '{}' not supported"sv}(mediaType)}
    {
    }

}
