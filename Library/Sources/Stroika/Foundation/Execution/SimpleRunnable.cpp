/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Configuration/StroikaConfig.h"

#include    "SimpleRunnable.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;




/*
 ********************************************************************************
 *************************** Execution::SimpleRunnable **************************
 ********************************************************************************
 */
void    SimpleRunnable::Run ()
{
    (fFun2CallOnce) (fArg);
}

void    SimpleRunnable::FakeZeroArg_ (void* arg)
{
    void (*realFunCall) ()  =   reinterpret_cast<void (*) ()> (arg);
    (realFunCall) ();
}
