/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "SharedPtr.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Memory;

conditional_t<Execution::kSpinLock_IsFasterThan_mutex, Execution::SpinLock, mutex> Private_::sSharedPtrCopyLock_;
