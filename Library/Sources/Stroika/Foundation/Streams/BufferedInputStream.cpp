/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Execution/Exceptions.h"

#include "BufferedInputStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 ************** Streams::BufferedInputStream::Private_ **************************
 ********************************************************************************
 */
[[noreturn]] void BufferedInputStream::Private_::ThrowCannotSeekFromEnd_ ()
{
    // should throw or assert?
    Throw (runtime_error{"cannot seek from end if length not known"});
}