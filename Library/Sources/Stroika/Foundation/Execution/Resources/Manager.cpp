/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Manager.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Resources;

/*
 ********************************************************************************
 ********************* Execution::Resources::Manager ****************************
 ********************************************************************************
 */
Accessor Manager::CheckedReadResource (const Name& name) const noexcept
{
    try {
        return fRep_->ReadResource (name);
    }
    catch (...) {
        AssertNotReached ();
        return Accessor{span<const byte>{}};
    }
}
