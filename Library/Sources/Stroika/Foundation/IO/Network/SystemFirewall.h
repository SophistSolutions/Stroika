/*
* Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
*/
#ifndef _Stroika_Foundation_IO_Network_SystemFirewall_h_
#define _Stroika_Foundation_IO_Network_SystemFirewall_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <compare>
#include <filesystem>

#if qPlatform_Windows
#include <netfw.h>
#endif

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/**
*/

namespace Stroika::Foundation::IO::Network::SystemFirewall {

    using namespace Stroika::Foundation;

    using Characters::String;

    /**
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     */
    struct Rule {
        String           fName;
        String           fDescription;
        String           fGroup;
        filesystem::path fApplication;
#if qPlatform_Windows
        NET_FW_PROFILE_TYPE2  fProfileMask;
        NET_FW_RULE_DIRECTION fDirection;
        NET_FW_IP_PROTOCOL_   fProtocol;
#endif
        String fLocalPorts;
        String fRemotePorts;
#if qPlatform_Windows
        NET_FW_ACTION fAction;
#endif
        bool fEnabled;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const Rule&) const = default;
    };

    /**
     *  Configure the OS/system firewall on the system where the application is being run.
     *
     *  Many apps must augment the system firewall in order to operate properly. Often (e.g. on windows)
     *  this requires admin access (running as root or administrator). These functions will often fail
     *  (exceptions) due to inadequate access permissions.
     *
     *  \note - this class contains no data, and is not copyable, and is just a proxy to talk to the OS
     *  storage of firewall rules.
     */
    class Manager {
    public:
        Manager () = default;

    public:
        /**
         *  Return true if created, and false if already there and no change needed. Throw if
         *  any errors (besides no work todo), such as not running 'access denined'.
         */
        nonvirtual bool Register (const Rule& rule);

    public:
        /**
         */
        nonvirtual optional<Rule> Lookup (const String& ruleName) const;

    public:
        /**
         */
        nonvirtual Traversal::Iterable<Rule> LookupByGroup (const String& groupName) const;

    public:
        /**
         */
        nonvirtual Traversal::Iterable<Rule> LookupAll () const;
    };

}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "SystemFirewall.inl"

#endif /*_Stroika_Foundation_IO_Network_SystemFirewall_h_*/
