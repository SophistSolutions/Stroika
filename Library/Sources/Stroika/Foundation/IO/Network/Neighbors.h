/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Neighbors_h_
#define _Stroika_Foundation_IO_Network_Neighbors_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Characters/String.h"
#include "../../Containers/Collection.h"
#include "../../Containers/Set.h"
#include "../../Time/Duration.h"

#include "Interface.h"
#include "InternetAddress.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
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
        /**
         *  Note, though there are no guarantees about the form of a hardware address, we generally prefer emitting
         *  things like ab:cd:ef... for ethernet addresses over what windows sometimes uses, ab-cd-ef...
         */
        struct Neighbor {
            InternetAddress         fInternetAddress;
            String                  fHardwareAddress;
            Interface::SystemIDType fInterfaceID;

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
#if qPlatform_Linux
                eProcNetArp,
#endif
            };
            optional<Containers::Set<Strategy>> fStategies;
            optional<Time::Duration> fMaxLatnecy; // if monitoring in the background (e.g. in a thread) this configures polling frequency but maybe ignored)

            // if true, assure results always up to date (as if by polling in the background)
            // If false, minimize resource usage, and just fetch data on GetNeighbors call
            // Defaults to false, and @todo NYI (when implemented, use IntervalTimer code)
            optional<bool> fMonitor;

            // sometimes entries will remain in arp table after they have expired (just with address deleted).
            // Probably best to not include those
            optional<bool> fIncludePurgedEntries;

            // On windows, we appear to get bogus hardware addresses for broadcast entries
            // defaults true
            optional<bool> fOmitAllFFHardwareAddresses;
        };

    public:
        /**
         */
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
