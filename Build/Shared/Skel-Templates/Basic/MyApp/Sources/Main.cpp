/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/CommandLine.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Execution;

int main (int argc, const char* argv[])
{
    CommandLine               cmdLine{argc, argv};
    Debug::TraceContextBumper ctx{"main", "argv={}"_f, cmdLine};
    return EXIT_SUCCESS;
}
