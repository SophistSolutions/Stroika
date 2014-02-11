/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/String_Constant.h"

#include    "WaitAbandonedException.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Execution;




/*
 ********************************************************************************
 ************************ WaitAbandonedException ********************************
 ********************************************************************************
 */
WaitAbandonedException::WaitAbandonedException ()
    : StringException (String_Constant (L"WAIT on object abandoned because the owning thread terminated"))
{
}
