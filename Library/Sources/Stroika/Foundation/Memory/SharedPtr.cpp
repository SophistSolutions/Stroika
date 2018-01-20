/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "SharedPtr.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Memory;

#if qStroika_Foundation_Execution_SpinLock_IsFasterThan_mutex
Execution::SpinLock Private_::sSharedPtrCopyLock_;
#else
mutex Private_::sSharedPtrCopyLock_;
#endif
