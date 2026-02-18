/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Frameworks/Service/Main.h"

#include "AppVersion.h"

#include "AppConfiguration.h"
#include "WSImpl.h"
#include "WebServer.h"

#include "Service.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Frameworks::Service;

using namespace Stroika::Samples::HTMLUI;
using namespace Stroika::Samples::HTMLUI::Service;

namespace {
    const Main::ServiceDescription kServiceDescription_{.fRegistrationName = "Stroika-Sample-HTMLUI Service"_k,
                                                        .fPrettyName       = "Stroika Sample HTMLUI Service"_k};
}

SampleAppServiceRep::SampleAppServiceRep (optional<uint16_t> portNumberOverride)
    : fPortNumberOverride_{portNumberOverride}
{
}

void SampleAppServiceRep::MainLoop (const std::function<void ()>& startedCB)
{
    /*
     *  This mainloop function is SUBTLE. READ CAREFULLY. It's slightly difficult to understand, but once you do,
     *  you will see its designed to avoid pitfalls with incomplete startup, and shutdown.
     *
     *  The lifetime of the service roughly matches the lifetime of this MainLoop. Terminating the service (myService --stop)
     *  will cause a Thread::AbortException to be sent to this MainLoop, so that it unwinds.
     *
     *  The simplest and safest way to construct this MainLoop is using RAII, referencing external
     *  'modules' which start and stop any needed threads and do whatever setup/shutdown is needed.
     */

    // Just so you get a clear message in the log that the service didn't startup.
    bool                    successfullyStarted{false};
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () {
        if (not successfullyStarted) {
            Logger::sThe.Log (Logger::eError, "Failed to successfully start service"_f);
        }
    });

    /*
     *  optional declareActivity object, so can just 'activate' it when we start to shutdown
     *  when the DeclareActivity is constructed/alive, then any exceptions thrown will report that as the current
     *  activity.
     */
    constexpr Activity                                           kShuttingDownServices_{"shutting down modules"sv};
    optional<DeclareActivity<decltype (kShuttingDownServices_)>> oDeclareActivity{};

    /*
     * define all your activators here, for modules activated by the service. For now, we just have the one (webserver).
     */
    WebServer myWebServer{fPortNumberOverride_}; // listen and dispatch while this object exists

    /*
     * At this point, all our components have been successfully initialized/started. So acknowledge that fact in the log,
     *and with the service control manager.
     */
    startedCB ();
    Logger::sThe.Log (Logger::eInfo, "{} (version {}) service started successfully"_f, kServiceDescription_.fPrettyName, AppVersion::kVersion);
    successfullyStarted = true;

    /*
     * the final object declared on the stack before we wait, so its the first run when we are handling the
     * thread abort exception, and unwinding this call. This prints a user message (to the log) and
     * sets the 'activity' to 'shutting down' so any exceptions cleaning up will more clearly report what was going on during the
     * failure (timeout exception).
     */
    [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([&] () {
        Execution::Thread::SuppressInterruptionInContext suppressSoWeActuallyShutDownOtherTaskWhenWereBeingShutDown;
        Logger::sThe.Log (Logger::eInfo, "Beginning service shutdown"_f);
        oDeclareActivity.emplace (&kShuttingDownServices_);
    });

    /*
     *  This thread will block here, and never go any further. When the service is terminated, WaitableEvent will
     *  abort (raise exception) and all the destructors on the stack (above) from this routine will get played
     *  backwards to cleanup.
     */
    WaitableEvent{}.Wait (); // until told to stop by abort exception

    AssertNotReached ();
}

Main::ServiceDescription SampleAppServiceRep::GetServiceDescription () const
{
    return kServiceDescription_;
}
