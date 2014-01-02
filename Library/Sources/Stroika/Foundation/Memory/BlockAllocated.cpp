/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "BlockAllocated.h"






using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Memory::Private;
using   namespace   Stroika::Foundation::Execution;

using   namespace   Execution;



mutex*    Private::sCritSection_  =   nullptr;



/*
 ********************************************************************************
 *********************** BlockAllocation_ModuleInit_ ****************************
 ********************************************************************************
 */
BlockAllocation_ModuleInit_::BlockAllocation_ModuleInit_ ()
{
    Require (sCritSection_ == nullptr);
    sCritSection_ = DEBUG_NEW mutex ();
}

BlockAllocation_ModuleInit_::~BlockAllocation_ModuleInit_ ()
{
    RequireNotNull (sCritSection_);
    delete sCritSection_;
    sCritSection_ = nullptr;
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
