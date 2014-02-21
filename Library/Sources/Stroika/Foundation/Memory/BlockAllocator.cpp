/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "BlockAllocator.h"






using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Memory::Private_;
using   namespace   Stroika::Foundation::Execution;

using   namespace   Execution;



Private_::LockType_*    Private_::sLock_  =   nullptr;



/*
 ********************************************************************************
 *********************** BlockAllocator_ModuleInit_ *****************************
 ********************************************************************************
 */
BlockAllocator_ModuleInit_::BlockAllocator_ModuleInit_ ()
{
    Require (sLock_ == nullptr);
    sLock_ = DEBUG_NEW Private_::LockType_ ();
}

BlockAllocator_ModuleInit_::~BlockAllocator_ModuleInit_ ()
{
    RequireNotNull (sLock_);
    delete sLock_;
    sLock_ = nullptr;
}




/*
 ********************************************************************************
 ************** Memory::MakeModuleDependency_BlockAllocator *********************
 ********************************************************************************
 */
Execution::ModuleDependency Memory::MakeModuleDependency_BlockAllocator ()
{
    return Execution::ModuleInitializer<Private_::BlockAllocator_ModuleInit_>::GetDependency ();
}
