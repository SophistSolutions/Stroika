/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_JSON_Pointer_h_
#define _Stroika_Foundation_DataExchange_JSON_Pointer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"

/**
 */
namespace Stroika::Foundation::DataExchange::JSON {

    using namespace Stroika::Foundation;

    using Characters::String;

    /**
     *  @see https://datatracker.ietf.org/doc/html/rfc6901/
     */
    using PointerType = String;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Pointer.inl"

#endif /*_Stroika_Foundation_DataExchange_JSON_Pointer_h_*/
