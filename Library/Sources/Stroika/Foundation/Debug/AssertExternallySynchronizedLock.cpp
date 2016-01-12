/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "AssertExternallySynchronizedLock.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Debug;




/*
 ********************************************************************************
 ****************** AssertExternallySynchronizedLock ****************************
 ********************************************************************************
 */
#if     qDebug
mutex       AssertExternallySynchronizedLock::sSharedLockThreadsMutex_;
#endif
