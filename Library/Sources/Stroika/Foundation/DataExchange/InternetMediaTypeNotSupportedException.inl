/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     *********************** InternetMediaTypeNotSupportedException *****************
     ********************************************************************************
     */
    inline InternetMediaTypeNotSupportedException::InternetMediaTypeNotSupportedException ()
        : Execution::RuntimeErrorException<>{"Internet Media Type not supported"sv}
    {
    }
    inline InternetMediaTypeNotSupportedException::InternetMediaTypeNotSupportedException (const InternetMediaType& mediaType)
        : Execution::RuntimeErrorException<>{Characters::FormatString<char>{"Internet Media Type {} not supported"sv}(mediaType)}
    {
    }

}
