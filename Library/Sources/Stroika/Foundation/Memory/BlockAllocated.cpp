/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "BlockAllocated.h"






using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Memory::Private;
using   namespace   Stroika::Foundation::Execution;

using   namespace   Execution;



Private::LockType_*    Private::sLock_  =   nullptr;



/*
 ********************************************************************************
 *********************** BlockAllocation_ModuleInit_ ****************************
 ********************************************************************************
 */
BlockAllocation_ModuleInit_::BlockAllocation_ModuleInit_ ()
{
    Require (sLock_ == nullptr);
    sLock_ = DEBUG_NEW Private::LockType_ ();
}

BlockAllocation_ModuleInit_::~BlockAllocation_ModuleInit_ ()
{
    RequireNotNull (sLock_);
    delete sLock_;
    sLock_ = nullptr;
}




/*
 ********************************************************************************
 ************** Memory::MakeModuleDependency_BlockAllocated *********************
 ********************************************************************************
 */
Execution::ModuleDependency Memory::MakeModuleDependency_BlockAllocated ()
{
    return Execution::ModuleInitializer<Private::BlockAllocation_ModuleInit_>::GetDependency ();
}
