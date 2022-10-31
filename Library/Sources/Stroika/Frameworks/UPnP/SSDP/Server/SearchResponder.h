/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_SearchResponder_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_SearchResponder_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../../Foundation/Characters/String.h"
#include "../../../../Foundation/Configuration/Common.h"
#include "../../../../Foundation/Execution/Thread.h"
#include "../../../../Foundation/Traversal/Iterable.h"

#include "../../Device.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Look at http://brisa.garage.maemo.org/doc/html/upnp/ssdp.html for example server API
 *
 *      @todo   This is a rough draft implementation. Quite incomplete (and not sure working, untested)
 */

namespace Stroika::Frameworks::UPnP::SSDP::Server {

    using Traversal::Iterable;

    /**
     *  Instantiating the class does nothing. You must invoke Run() to start the search. And then
     *  it runs in the background (with its own private thread) until the SearchResponder object is
     *  destroyed.
     */
    class SearchResponder {
    public:
        SearchResponder ()                                       = default;
        SearchResponder (const SearchResponder&)                 = delete;
        const SearchResponder operator= (const SearchResponder&) = delete;
        ~SearchResponder ()                                      = default;

    public:
        nonvirtual void Run (const Iterable<Advertisement>& advertisements);

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
