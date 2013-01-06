/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Execution/CriticalSection.h"

#include    "BlockAllocated.h"






using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Memory::Private;
using   namespace   Stroika::Foundation::Execution;

using   namespace   Execution;

using   Execution::CriticalSection;


CriticalSection*    Private::sCritSection_  =   nullptr;


BlockAllocation_ModuleInit_::BlockAllocation_ModuleInit_ ()
{
#if     !qDESIGN_FLAW_WITH_MODULE_INIT_DEPENDENCIES_FROM_CPP_FILE
    Require (sCritSection_ == nullptr);
    sCritSection_ = DEBUG_NEW CriticalSection ();
#endif
}

BlockAllocation_ModuleInit_::~BlockAllocation_ModuleInit_ ()
{
#if     !qDESIGN_FLAW_WITH_MODULE_INIT_DEPENDENCIES_FROM_CPP_FILE
    RequireNotNull (sCritSection_);
#endif
    delete sCritSection_;
    sCritSection_ = nullptr;
}



Execution::DependableModule Memory::_BlockAllocated_    =   Execution::ModuleInitializer<Private::BlockAllocation_ModuleInit_>::GetDependableModule ();





#if     qAllowBlockAllocation
void*   Private::sSizeof_4_NextLink     =   nullptr;
void*   Private::sSizeof_8_NextLink     =   nullptr;
void*   Private::sSizeof_12_NextLink    =   nullptr;
void*   Private::sSizeof_16_NextLink    =   nullptr;
void*   Private::sSizeof_20_NextLink    =   nullptr;
void*   Private::sSizeof_24_NextLink    =   nullptr;
void*   Private::sSizeof_28_NextLink    =   nullptr;
void*   Private::sSizeof_32_NextLink    =   nullptr;
void*   Private::sSizeof_36_NextLink    =   nullptr;
void*   Private::sSizeof_40_NextLink    =   nullptr;
void*   Private::sSizeof_44_NextLink    =   nullptr;
void*   Private::sSizeof_48_NextLink    =   nullptr;
void*   Private::sSizeof_52_NextLink    =   nullptr;
void*   Private::sSizeof_56_NextLink    =   nullptr;
void*   Private::sSizeof_60_NextLink    =   nullptr;
void*   Private::sSizeof_64_NextLink    =   nullptr;
void*   Private::sSizeof_68_NextLink    =   nullptr;
void*   Private::sSizeof_72_NextLink    =   nullptr;
void*   Private::sSizeof_76_NextLink    =   nullptr;
void*   Private::sSizeof_80_NextLink    =   nullptr;
#endif

