/*
* Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
*/
#ifndef _Stroika_Foundation_IO_Network_SystemFirewall_h_
#define _Stroika_Foundation_IO_Network_SystemFirewall_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

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
#if __cpp_impl_three_way_comparison < 201907
        bool operator== (const Rule& rhs) const
        {
            if (fName != rhs.fName) {
                return false;
            }
            if (fDescription != rhs.fDescription) {
                return false;
            }
            if (fGroup != rhs.fGroup) {
                return false;
            }
            if (fApplication != rhs.fApplication) {
                return false;
            }
#if qPlatform_Windows
            if (fProfileMask != rhs.fProfileMask) {
                return false;
            }
            if (fDirection != rhs.fDirection) {
                return false;
            }
            if (fProtocol != rhs.fProtocol) {
                return false;
            }
#endif
            if (fLocalPorts != rhs.fLocalPorts) {
                return false;
            }
            if (fRemotePorts != rhs.fRemotePorts) {
                return false;
            }
#if qPlatform_Windows
            if (fAction != rhs.fAction) {
                return false;
            }
#endif
            if (fEnabled != rhs.fEnabled) {
                return false;
            }
            return true;
        }
        bool operator!= (const Rule& rhs) const
        {
            return not(*this == rhs);
        }
#else
        nonvirtual strong_ordering operator<=> (const Rule&) const = default;
#endif
    };

    /**
     *  Configure the OS/system firewall on the system where the application is being run.
     *
     *  Many apps must augment the system firewall in order to operate properly. Often (e.g. on windows)
     *  this requires admin access (running as root or administrator). These functions will often fail
     *  (exceptions) due to inadequate access permissions.
     */
    class Manager {
    public:
        static Manager sThe;

    private:
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
        nonvirtual optional<Rule> Lookup (const String& ruleName);

    public:
        /**
         */
        nonvirtual Traversal::Iterable<Rule> LookupByGroup (const String& groupName);

    public:
        /**
         */
        nonvirtual Traversal::Iterable<Rule> LookupAll ();
    };

}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "SystemFirewall.inl"

#endif /*_Stroika_Foundation_IO_Network_SystemFirewall_h_*/
