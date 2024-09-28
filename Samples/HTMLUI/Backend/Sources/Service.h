/*
* Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
*/
#ifndef _RFL_AskHealthFrame_WebServices_Service_h_
#define _RFL_AskHealthFrame_WebServices_Service_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/Service/Main.h"

/**
*/

namespace Stroika::Samples::HTMLUI ::Service {

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

    /**
         *  This class contains the 'main service loop' - which is how a service is started and shut down (and modules tied together).
         */
    struct SampleAppServiceRep : Main::IApplicationRep {
        SampleAppServiceRep (optional<uint16_t> portNumberOverride);
        virtual ~SampleAppServiceRep () = default;

    public:
        virtual void MainLoop (const std::function<void ()>& startedCB) override;

    public:
        virtual Main::ServiceDescription GetServiceDescription () const override;

    private:
        optional<uint16_t> fPortNumberOverride_;
    };

}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "Service.inl"

#endif /*_RFL_AskHealthFrame_WebServices_Service_h_*/
