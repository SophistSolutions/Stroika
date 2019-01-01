/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Characters/String_Constant.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Frameworks/Service/Main.h"

#include "AppVersion.h"

#include "Service.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Service;

using Characters::String_Constant;
using Execution::Thread;

using namespace Samples::Service;

#if qUseLogger
#include "Stroika/Foundation/Execution/Logger.h"
using Execution::Logger;
#endif

namespace {
    const Main::ServiceDescription kServiceDescription_{
        String_Constant{L"Stroika-Sample-Service"},
        String_Constant{L"Stroika Sample Service"}};
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
            : fSomeOtherTaskDoingRealWork_ (Thread::CleanupPtr::eAbortBeforeWaiting,
                                            Thread::New (
                                                []() {
                                                    Execution::Sleep (1 * 24 * 60 * 60); // wait 1 day ... simple test....
                                                },
                                                Thread::eAutoStart))
        {
        }
        ~SomeModuleALikeWebServer_ () = default;
        Thread::CleanupPtr fSomeOtherTaskDoingRealWork_;
    };
}

void SampleAppServiceRep::MainLoop (const std::function<void()>& startedCB)
{
    /*
     *  The lifetime of the service roughly matches the lifetime of this MainLoop. Terminating the service (myService --stop)
     *  will cause a Thread::AbortException to be sent to this MainLoop, so that it unwinds.
     *
     *  The simplest and safest way to construct this MainLoop is using RAII, referencing external
     *  'modules' which start and stop any needed threads and do whatever setup/shutdown is needed.
     */

#if qUseLogger
    // Just so you get a clear message in the log that the service didn't startup. The things that actually caused the problem should
    bool                    successfullyStarted{false};
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([&]() {
        if (not successfullyStarted) {
            Logger::Get ().Log (Logger::Priority::eError, L"Failed to successfully start service");
        }
    });
#endif

    /*
     *  Startup modules.
     *
     *  \note initialized in the order given here, and shutdown in the reverse order, so order really does matter.
     *        Put the more general ones with fewest dependencies first.
     */
    SomeModuleALikeWebServer_ moduleA;
    SomeModuleALikeWebServer_ moduleB; // typically of a differnt type

    startedCB (); // Notify service control mgr that the service has started

#if qUseLogger
    Logger::Get ().Log (Logger::Priority::eInfo, L"%s (version %s) service started successfully", kServiceDescription_.fPrettyName.c_str (), Characters::ToString (AppVersion::kVersion).c_str ());
    successfullyStarted = true;

    // the final object delcared on the stack before we wait, so its the first run when we are handling the
    // thread aboort exception, and unwinding this call.
    [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([&]() {
        Logger::Get ().Log (Logger::Priority::eInfo, L"Beginning service shutdown");
    });
#endif

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
