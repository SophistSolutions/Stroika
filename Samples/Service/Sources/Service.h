/*
* Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
*/
#ifndef _Samples_Service_h_
#define _Samples_Service_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/Service/Main.h"

/**
*/

namespace Samples {
    namespace Service {

        using namespace Stroika::Foundation;
        using namespace Stroika::Frameworks::Service;

/*
 *  Almost always would want to use logger with a service. But demarcate so clear what is 'service' demo
 *  and what is logger demo.
 *
 *  To test, on many systems, you can do
 *      tail -f /var/log/syslog (in one window - note - depending on your system, the logfile could be elsewhere)
 *  and
 *      Samples_SampleService -start
 *      Samples_SampleService -status
 *      Samples_SampleService -stop
 *  and see the log messages appear in the logfile.
 */
#ifndef qUseLogger
#define qUseLogger 1
#endif

        /**
         *  This class contains the 'main service loop' - which is how a service is started and shut down (and modules tied together).
         */
        struct SampleAppServiceRep : Main::IApplicationRep {
            SampleAppServiceRep ()          = default;
            virtual ~SampleAppServiceRep () = default;

        public:
            virtual void MainLoop (const std::function<void()>& startedCB) override;

        public:
            virtual Main::ServiceDescription GetServiceDescription () const override;
        };
    }
}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "Service.inl"

#endif /*_Samples_Service_h_*/
