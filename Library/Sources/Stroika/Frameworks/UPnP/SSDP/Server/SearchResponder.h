/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_SearchResponder_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_SearchResponder_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

#include "Stroika/Frameworks/UPnP/Device.h"

/*
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Look at http://brisa.garage.maemo.org/doc/html/upnp/ssdp.html for example server API
 */

namespace Stroika::Frameworks::UPnP::SSDP::Server {

    using Traversal::Iterable;

    /**
     *  Instantiating the class starts the (background) search automatically, and it continues
     *  until the SearchResponder object is destroyed.
     * 
     *  \note - this behavior differs from Stroika 2.1, where you had to explicitly call Run ()
     * 
     *  \note this uses its own thread, rather than using IntervalTimer, because it waits on input
     *        from the network, and must always be running/waiting
     */
    class SearchResponder {
    public:
        /**
         */
        SearchResponder (const Iterable<Advertisement>& advertisements,
                         IO::Network::InternetProtocol::IP::IPVersionSupport ipVersion = IO::Network::InternetProtocol::IP::IPVersionSupport::eDEFAULT);
        SearchResponder (const SearchResponder&) = delete;

    public:
        const SearchResponder operator= (const SearchResponder&) = delete;

    public:
        ~SearchResponder () = default;

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
#include "SearchResponder.inl"

#endif /*_Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_*/
