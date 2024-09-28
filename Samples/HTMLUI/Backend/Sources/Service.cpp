/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
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

using Execution::Logger;

namespace {
    const Main::ServiceDescription kServiceDescription_{"Stroika-Sample-HTMLUI Service"_k, "Stroika-Sample-HTMLUI Service"_k};
}

namespace {
    //
    // Typically have one like this for each major module your service manages.
    //
    // These get defined/declared in that module, and just referenced here
    //
    struct SomeModuleALikeWebServer_ {
        // initialize that service//module here, including starting any threads
        SomeModuleALikeWebServer_ ()
            : fSomeOtherTaskDoingRealWork_{Thread::CleanupPtr::eAbortBeforeWaiting, Thread::New (
                                                                                        [] () {
                                                                                            Execution::Sleep (24h); // wait 1 day ... simple test....
                                                                                        },
                                                                                        Thread::eAutoStart)}
        {
        }
        ~SomeModuleALikeWebServer_ () = default;
        Thread::CleanupPtr fSomeOtherTaskDoingRealWork_;
    };
}
SampleAppServiceRep::SampleAppServiceRep (optional<uint16_t> portNumberOverride)
    : fPortNumberOverride_{portNumberOverride}
{
}
void SampleAppServiceRep::MainLoop (const std::function<void ()>& startedCB)
{
    /*
     *  The lifetime of the service roughly matches the lifetime of this MainLoop. Terminating the service (myService --stop)
     *  will cause a Thread::AbortException to be sent to this MainLoop, so that it unwinds.
     *
     *  The simplest and safest way to construct this MainLoop is using RAII, referencing external
     *  'modules' which start and stop any needed threads and do whatever setup/shutdown is needed.
     */

    // Just so you get a clear message in the log that the service didn't startup. The things that actually caused the problem should
    bool                    successfullyStarted{false};
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () {
        if (not successfullyStarted) {
            Logger::sThe.Log (Logger::eError, "Failed to successfully start service"_f);
        }
    });

    /*
     *  Startup modules.
     *
     *  \note initialized in the order given here, and shutdown in the reverse order, so order really does matter.
     *        Put the more general ones with fewest dependencies first.
     */
    //  SomeModuleALikeWebServer_ moduleA;
    //SomeModuleALikeWebServer_ moduleB; // typically of a differnt type

    startedCB (); // Notify service control mgr that the service has started

    Logger::sThe.Log (Logger::eInfo, "{} (version {}) service started successfully"_f, kServiceDescription_.fPrettyName,
                      Characters::ToString (AppVersion::kVersion));
    successfullyStarted = true;

    // the final object delcared on the stack before we wait, so its the first run when we are handling the
    // thread aboort exception, and unwinding this call.
    [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([&] () { Logger::sThe.Log (Logger::eInfo, "Beginning service shutdown"_f); });

    WebServer myWebServer{fPortNumberOverride_}; // listen and dispatch while this object exists

    /*
     *  This thread will block here, and never go any further. When the service is terminated, WaitableEvent will
     *  abort (raise exception) and all the destructors on the stack (above) from this routine will get played
     *  backewards to cleanup.
     */
    Execution::WaitableEvent{}.Wait (); // until told to stop by abort exception

    AssertNotReached ();
}

Main::ServiceDescription SampleAppServiceRep::GetServiceDescription () const
{
    return kServiceDescription_;
}
