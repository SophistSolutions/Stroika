/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"

#include "AppConfiguration.h"
#include "MoreConfiguration.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO::FileSystem;

using namespace StroikaSample;
using namespace StroikaSample::AppSettings;

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Debug::TraceContextBumper ctx{
        Stroika_Foundation_Debug_OptionalizeTraceArgs ("main", "argv={}"_f, Characters::ToString (vector<const char*>{argv, argv + argc}))};

    Logger::Activator logMgrActivator; // OptionsFile uses this by default...'

    // for a test app, writing to stdout, seems a plausible default...
    Logger::sThe.AddAppender (
        make_shared<Logger::StreamAppender> (FileOutputStream::New (STDOUT_FILENO, FileStream::AdoptFDPolicy::eDisconnectOnDestruction)));

    // Simple example using OpensFile, and ModuleGetterSetter
    [[maybe_unused]] uint16_t usePort = gAppConfiguration->WebServerPort.value_or (AppConfigurationType::kWebServerPort_Default);

    // Fancier examples
    TestUse1 ();
    TestUse2 ();
    TestUse3 ();
    TestUse4 ();

    return EXIT_SUCCESS;
}
