/*
* Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
*/
#ifndef _Stroika_Foundation_IO_Network_Firewall_h_
#define _Stroika_Foundation_IO_Network_Firewall_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#if qPlatform_Windows
#include <netfw.h>
#endif

#include "Stroika/Foundation/Characters/String.h"

/**
*/

namespace Stroika::Foundation::IO::Network::Firewall {

    using namespace Stroika::Foundation;

    using Characters::String;

    /**
     */
    struct Rule {
        String fName;
        String fDescription;
        String fGroup;
        String fApplication;
#if qPlatform_Windows
        NET_FW_PROFILE_TYPE2  fProfileMask;
        NET_FW_RULE_DIRECTION fDirection;
        NET_FW_IP_PROTOCOL_   fProtocol;
#endif
        String fPorts;
#if qPlatform_Windows
        NET_FW_ACTION fAction;
#endif
        bool fEnabled;
    };

    /**
     *  Configure the OS/system firewall on the system where the application is being run.
     *
     *  Many apps must augment the system firewall in order to operate properly. Often (e.g. on windows)
     *  this requires admin access (running as root or administrator). These functions will often fail
     *  (exceptions) due to inadequate access permissions.
     */
    class SystemFirewall {
    public:
        static SystemFirewall sThe;

    private:
        SystemFirewall () = default;

    public:
        /**
         *  Return true if created, and false if already there and no change needed. Throw if
         *  any errors (besides no work todo), such as not running 'access denined'.
         */
        nonvirtual bool Register (const Rule& rule);

    public:
        /**
         */
        nonvirtual optional<Rule> Lookup (const String& ruleName);
    };

}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "Firewall.inl"

#endif /*_Stroika_Foundation_IO_Network_Firewall_h_*/
