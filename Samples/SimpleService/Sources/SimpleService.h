/*
* Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
*/
#ifndef _Samples_SimpleService_h_
#define _Samples_SimpleService_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Execution/Thread.h"

#include "Stroika/Frameworks/Service/Main.h"

/**
*/

namespace Samples {
    namespace SimpleService {

        using namespace Stroika::Frameworks::Service;

/*
 *  Almost always would want to use logger with a service. But demarcate so clear what is 'service' demo
 *  and what is logger demo.
 *
 *  To test, on many systems, you can do
 *      tail -f /var/log/syslog (in one window - note - depending on your system, the logfile could be elsewhere)
 *  and
 *      Samples_SimpleService -start
 *      Samples_SimpleService -status
 *      Samples_SimpleService -stop
 *  and see the log messages appear in the logfile.
 */
#ifndef qUseLogger
#define qUseLogger 1
#endif

        using namespace Stroika::Foundation;

        struct SampleAppServiceRep : Main::IApplicationRep {
            SampleAppServiceRep ()          = default;
            virtual ~SampleAppServiceRep () = default;

        public:
            virtual void MainLoop (const std::function<void()>& startedCB) override;

        public:
            virtual Main::ServiceDescription GetServiceDescription () const override;

        public:
            // No NEED to use this technique, but its an easy way to have as many paralell process tasks
            // ask you want running while your main task does essentially nothing but cleanup when the
            // service shuts down
            Execution::Thread fSomeOtherTaskDoingRealWork;
        };
    }
}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "SimpleService.inl"

#endif /*_Samples_SimpleService_h_*/
