/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"

#include "AppConfiguration.h"
#include "MoreConfiguration.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace StroikaSample;
using namespace StroikaSample::AppSettings;

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Debug::TraceContextBumper ctx{
        Stroika_Foundation_Debug_OptionalizeTraceArgs ("main", "argv={}"_f, Characters::ToString (vector<const char*>{argv, argv + argc}))};

    // Simplest example
    [[maybe_unused]] uint16_t usePort = gAppConfiguration->WebServerPort.value_or (AppConfigurationType::kWebServerPort_Default);

    // Fancier examples
    TestUse1 ();
    TestUse2 ();
    TestUse3 ();
    TestUse4 ();

    return EXIT_SUCCESS;
}
