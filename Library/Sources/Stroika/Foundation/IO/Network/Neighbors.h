/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Neighbors_h_
#define _Stroika_Foundation_IO_Network_Neighbors_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Characters/String.h"
#include "../../Containers/Collection.h"
#include "../../Containers/Set.h"

#include "InternetAddress.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 */

namespace Stroika::Foundation::IO::Network {

    /**
     *  \brief Monitor - either immediately or in the background fetches a list of network neighbors (1 hop away network devices).
     *         Also fetches their hardware address (so like 'arp' kind of but may employ other means)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class NeighborsMonitor {
    public:
        struct Neighbor {
            InternetAddress fInternetAddress;
            String          fHardwareAddress;

        public:
            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

    public:
        /**
         */
        struct Options {
            enum class Strategy {
                eArpProgram,
                eProc
            };
            optional<Containers::Set<Strategy>> fStategies;
            optional<Time::Duration>            fMaxLatnecy; // if monitoring in the background (e.g. in a thread) this configures polling frequency but maybe ignored)

            // if true, assure results always up to date (as if by polling in the background)
            // If false, minimize resource usage, and just fetch data on GetNeighbors call
            optional<bool> fMonitor;

            // sometimes entries will remain in arp table after they have expired (just with address deleted).
            // Probably best to not include those
            optional<bool> fIncludePurgedEntries;
        };

    public:
        NeighborsMonitor (const Options& options = {});

    public:
        /**
         */
        nonvirtual Containers::Collection<Neighbor> GetNeighbors () const;

    private:
        class Rep_;
        shared_ptr<Rep_> fRep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Neighbors.inl"

#endif /*_Stroika_Foundation_IO_Network_Neighbors_h_*/
