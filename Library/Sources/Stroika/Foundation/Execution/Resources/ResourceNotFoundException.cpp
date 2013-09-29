/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "ResourceNotFoundException.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;





/*
 ********************************************************************************
 ************************ OperationNotSupportedException ************************
 ********************************************************************************
 */
ResourceNotFoundException::ResourceNotFoundException ()
    : StringException (L"Resource Not Found")
{
}
