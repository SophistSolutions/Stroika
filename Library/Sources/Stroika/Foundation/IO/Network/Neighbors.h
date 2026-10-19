/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Neighbors_h_
#define _Stroika_Foundation_IO_Network_Neighbors_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/Time/Duration.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
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
#if qStroika_Platform_Linux
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
            bool fIncludePurgedEntries{false};

            // On windows, we appear to get bogus hardware addresses for broadcast entries
            bool fOmitAllFFHardwareAddresses{true};
        };

    public:
        /**
         *  \note Two constructors, not one with a default argument (= {}): within NeighborsMonitor, that default argument cannot
         *        use the member initializers of Options, nested in it - g++ and clang refuse it (as of g++ 16, clang 22), MSVC
         *        allows it, and the standard has not settled it (https://cplusplus.github.io/CWG/issues/2335.html).
         */
        NeighborsMonitor ();
        NeighborsMonitor (const Options& options); ///< \brief Monitoring as options says

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
