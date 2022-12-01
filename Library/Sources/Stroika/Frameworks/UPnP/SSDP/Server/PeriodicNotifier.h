/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../../Foundation/Characters/String.h"
#include "../../../../Foundation/Configuration/Common.h"
#include "../../../../Foundation/Execution/Thread.h"
#include "../../../../Foundation/Traversal/Iterable.h"

#include "../../Device.h"
#include "../Advertisement.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Look at http://brisa.garage.maemo.org/doc/html/upnp/ssdp.html for example server API
 *
 *      @todo   Did rough draft implementation. Works for some simple cases.
 */

namespace Stroika::Frameworks::UPnP::SSDP::Server {

    using Traversal::Iterable;

    /**
     *  A big part of SSDP server functinality is to send periodic notifications of the Device description
     *
     *  Instantiating the class starts the (background) notifications automatically, and they
     *  continue until the PeriodicNotifier object is destroyed.
     * 
     *  \note - this behavior differs from Stroika 2.1, where you had to explicitly call Run ()
     */
    class PeriodicNotifier {
    public:
        // Very primitive definition - should refine - read details on spec on this...
        struct FrequencyInfo {
            Time::DurationSecondsType fRepeatInterval = 3 * 60.0;
        };

    public:
        PeriodicNotifier (const Iterable<Advertisement>& advertisements, const FrequencyInfo& fi);
        PeriodicNotifier (const PeriodicNotifier&) = delete;

    public:
        const PeriodicNotifier operator= (const PeriodicNotifier&) = delete;

    public:
        ~PeriodicNotifier () = default;

#if 0
        //...
        //Get/Set supported DeviceEntries ();

        //Get/Set Refresh/MaxAge (default is autocompute refresh pace based on maxage)

        // smart ptr to one of these - caller keeps it around, it runs in its own
        // thread as needed, does responses etc.
#endif
    private:
        Execution::Thread::CleanupPtr fListenThread_{Execution::Thread::CleanupPtr::eAbortBeforeWaiting};
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PeriodicNotifier.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_*/
