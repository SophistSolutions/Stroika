/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaTypeNotSupportedException_inl_
#define _Stroika_Foundation_DataExchange_InternetMediaTypeNotSupportedException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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
        : Execution::RuntimeErrorException<>{Characters::FormatString<wchar_t>{L"Internet Media Type {} not supported"}(mediaType)}
    {
    }

}

#endif /*_Stroika_Foundation_DataExchange_InternetMediaTypeNotSupportedException_inl_*/
