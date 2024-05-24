/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#if qPlatform_Windows
#include <windows.h>
#endif

#if qPlatform_Windows
#include "Stroika/Foundation/Characters/Platform/Windows/SmartBSTR.h"
#endif
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Finally.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/COM.h"
#include "Stroika/Foundation/Execution/Platform/Windows/HRESULTErrorException.h"
#endif

#include "SystemFirewall.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
#if qPlatform_Windows
using namespace Stroika::Foundation::Characters::Platform::Windows;
#endif
using namespace Stroika::Foundation::Containers;
#if qPlatform_Windows
using namespace Stroika::Foundation::Execution::Platform::Windows;
#endif
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::SystemFirewall;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qPlatform_Windows
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#endif

/*
 ********************************************************************************
 ******************** IO::Network::SystemFirewall::Rule *************************
 ********************************************************************************
 */
String Rule::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Name: "sv << fName << ","sv;
    sb << "Description: "sv << fDescription << ","sv;
    sb << "Application: "sv << fApplication << ","sv;
#if qPlatform_Windows
    sb << "ProfileMask: "sv << (int)fProfileMask << ","sv;
    sb << "Direction: "sv << (int)fDirection << ","sv;
    sb << "Protocol: "sv << (int)fProtocol << ","sv;
#endif
    sb << "LocalPorts: "sv << fLocalPorts << ","sv;
    sb << "RemotePorts: "sv << fRemotePorts << ","sv;
#if qPlatform_Windows
    sb << "Action: "sv << (int)fAction << ","sv;
#endif
    sb << "Enabled: "sv << fEnabled << ","sv;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ***************** IO::Network::SystemFirewall::Manager *************************
 ********************************************************************************
 */
#if qPlatform_Windows
namespace {
    Rule ReadRule_ (INetFwRule* pFwRule)
    {
        AssertNotNull (pFwRule);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"{}ReadRule_"};
#endif
        BSTR                    name        = nullptr;
        BSTR                    desc        = nullptr;
        BSTR                    group       = nullptr;
        BSTR                    application = nullptr;
        BSTR                    localPorts  = nullptr;
        BSTR                    remotePorts = nullptr;
        [[maybe_unused]] auto&& cleanup     = Execution::Finally ([=] () noexcept {
            if (name != nullptr) {
                ::SysFreeString (name);
            }
            if (desc != nullptr) {
                ::SysFreeString (desc);
            }
            if (group != nullptr) {
                ::SysFreeString (group);
            }
            if (application != nullptr) {
                ::SysFreeString (application);
            }
            if (localPorts != nullptr) {
                ::SysFreeString (localPorts);
            }
            if (remotePorts != nullptr) {
                ::SysFreeString (remotePorts);
            }
        });
        ThrowIfErrorHRESULT (pFwRule->get_Name (&name));
        ThrowIfErrorHRESULT (pFwRule->get_Description (&desc));
        ThrowIfErrorHRESULT (pFwRule->get_Grouping (&group));
        ThrowIfErrorHRESULT (pFwRule->get_ApplicationName (&application));
        long profileMask = 0;
        ThrowIfErrorHRESULT (pFwRule->get_Profiles (&profileMask));
        NET_FW_RULE_DIRECTION direction = NET_FW_RULE_DIR_MAX;
        ThrowIfErrorHRESULT (pFwRule->get_Direction (&direction));
        LONG protocol = 0;
        ThrowIfErrorHRESULT (pFwRule->get_Protocol (&protocol));
        ThrowIfErrorHRESULT (pFwRule->get_LocalPorts (&localPorts));
        ThrowIfErrorHRESULT (pFwRule->get_RemotePorts (&remotePorts));
        NET_FW_ACTION action = NET_FW_ACTION_MAX;
        ThrowIfErrorHRESULT (pFwRule->get_Action (&action));
        VARIANT_BOOL enabled = VARIANT_FALSE;
        ThrowIfErrorHRESULT (pFwRule->get_Enabled (&enabled));
        return Rule{name == nullptr ? wstring{} : wstring{name},
                    desc == nullptr ? wstring{} : wstring{desc},
                    group == nullptr ? wstring{} : wstring{group},
                    application == nullptr ? wstring{} : wstring{application},
                    (NET_FW_PROFILE_TYPE2)(profileMask),
                    direction,
                    (NET_FW_IP_PROTOCOL_)(protocol),
                    localPorts == nullptr ? wstring{} : wstring{localPorts},
                    remotePorts == nullptr ? wstring{} : wstring{remotePorts},
                    action,
                    enabled != VARIANT_FALSE};
    }
    optional<Rule> ReadRule_ (INetFwRules* pFwRules, const String& ruleName)
    {
        AssertNotNull (pFwRules);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"{}ReadRule_"};
#endif
        INetFwRule*             pFwRule           = nullptr;
        [[maybe_unused]] auto&& cleanupCOMObjects = Execution::Finally ([=] () noexcept {
            if (pFwRule != nullptr) {
                pFwRule->Release ();
            }
        });
        HRESULT                 hr                = pFwRules->Item (SmartBSTR{ruleName.As<wstring> ().c_str ()}, &pFwRule);
        if (hr == S_OK and pFwRule != nullptr) {
            return ReadRule_ (pFwRule);
        }
        return nullopt;
    }
}
#endif

bool SystemFirewall::Manager::Register (const Rule& rule)
{
    Debug::TraceContextBumper ctx{"SystemFirewall::Manager::Register", "rule={}"_f, rule};

    for (const auto& r : LookupByGroup (rule.fGroup)) {
        if (r == rule) {
            DbgTrace ("run unchanged, so returning false"_f);
            return false;
        }
    }

#if qPlatform_Windows
    COMInitializer          comInitializeContext{COINIT_APARTMENTTHREADED};
    INetFwPolicy2*          pNetFwPolicy2     = nullptr;
    INetFwRules*            pFwRules          = nullptr;
    INetFwRule*             pFwRule           = nullptr;
    [[maybe_unused]] auto&& cleanupCOMObjects = Execution::Finally ([=] () noexcept {
        if (pFwRule != nullptr) {
            pFwRule->Release ();
        }
        if (pFwRules != nullptr) {
            pFwRules->Release ();
        }
        if (pNetFwPolicy2 != nullptr) {
            pNetFwPolicy2->Release ();
        }
    });

    // Retrieve INetFwPolicy2
    ThrowIfErrorHRESULT (::CoCreateInstance (__uuidof (NetFwPolicy2), nullptr, CLSCTX_INPROC_SERVER, __uuidof (INetFwPolicy2), (void**)&pNetFwPolicy2));

    // Retrieve INetFwRules
    ThrowIfErrorHRESULT (pNetFwPolicy2->get_Rules (&pFwRules));

    // Create a new Firewall Rule object.
    ThrowIfErrorHRESULT (::CoCreateInstance (__uuidof (NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof (INetFwRule), (void**)&pFwRule));

    // Populate the Firewall Rule object
    ThrowIfErrorHRESULT (pFwRule->put_Name (SmartBSTR{rule.fName.As<wstring> ().c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_Description (SmartBSTR{rule.fDescription.As<wstring> ().c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_ApplicationName (SmartBSTR{rule.fApplication.c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_Protocol (rule.fProtocol));
    ThrowIfErrorHRESULT (pFwRule->put_LocalPorts (SmartBSTR{rule.fLocalPorts.As<wstring> ().c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_RemotePorts (SmartBSTR{rule.fRemotePorts.As<wstring> ().c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_Direction (rule.fDirection));
    ThrowIfErrorHRESULT (pFwRule->put_Grouping (SmartBSTR{rule.fGroup.As<wstring> ().c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_Profiles (rule.fProfileMask));
    ThrowIfErrorHRESULT (pFwRule->put_Action (rule.fAction));
    ThrowIfErrorHRESULT (pFwRule->put_Enabled (rule.fEnabled ? VARIANT_TRUE : VARIANT_FALSE));

    // Add the Firewall Rule
    DbgTrace ("Updating firewall rule"_f);
    ThrowIfErrorHRESULT (pFwRules->Add (pFwRule));
#endif
    return true;
}

optional<Rule> SystemFirewall::Manager::Lookup (const String& ruleName) const
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("SystemFirewall::Manager::Lookup", "ruleName={}"_f, ruleName)};
#if qPlatform_Windows
    COMInitializer          comInitializeContext{COINIT_APARTMENTTHREADED};
    INetFwPolicy2*          pNetFwPolicy2     = nullptr;
    INetFwRules*            pFwRules          = nullptr;
    [[maybe_unused]] auto&& cleanupCOMObjects = Execution::Finally ([=] () noexcept {
        if (pFwRules != nullptr) {
            pFwRules->Release ();
        }
        if (pNetFwPolicy2 != nullptr) {
            pNetFwPolicy2->Release ();
        }
    });

    // Retrieve INetFwPolicy2
    ThrowIfErrorHRESULT (::CoCreateInstance (__uuidof (NetFwPolicy2), nullptr, CLSCTX_INPROC_SERVER, __uuidof (INetFwPolicy2), (void**)&pNetFwPolicy2));

    // Retrieve INetFwRules
    ThrowIfErrorHRESULT (pNetFwPolicy2->get_Rules (&pFwRules));
    return ReadRule_ (pFwRules, ruleName);
#endif
    return nullopt;
}

Traversal::Iterable<Rule> SystemFirewall::Manager::LookupByGroup (const String& groupName) const
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("SystemFirewall::Manager::LookupByGroup", "groupName={}"_f, groupName)};
    Collection<Rule> rules;
    for (const Rule& r : LookupAll ()) {
        if (r.fGroup == groupName) {
            rules += r;
        }
    }
    return move (rules);
}

Traversal::Iterable<Rule> SystemFirewall::Manager::LookupAll () const
{
    Debug::TraceContextBumper ctx{"SystemFirewall::Manager::LookupAll"};
    Collection<Rule>          rules;
#if qPlatform_Windows
    COMInitializer          comInitializeContext{COINIT_APARTMENTTHREADED};
    INetFwPolicy2*          pNetFwPolicy2     = nullptr;
    INetFwRules*            pFwRules          = nullptr;
    IEnumVARIANT*           pEnum             = nullptr;
    [[maybe_unused]] auto&& cleanupCOMObjects = Execution::Finally ([=] () noexcept {
        if (pEnum != nullptr) {
            pEnum->Release ();
        }
        if (pFwRules != nullptr) {
            pFwRules->Release ();
        }
        if (pNetFwPolicy2 != nullptr) {
            pNetFwPolicy2->Release ();
        }
    });

    // Retrieve INetFwPolicy2
    ThrowIfErrorHRESULT (::CoCreateInstance (__uuidof (NetFwPolicy2), nullptr, CLSCTX_INPROC_SERVER, __uuidof (INetFwPolicy2), (void**)&pNetFwPolicy2));

    // Retrieve INetFwRules
    ThrowIfErrorHRESULT (pNetFwPolicy2->get_Rules (&pFwRules));

    ThrowIfErrorHRESULT (pFwRules->get__NewEnum ((IUnknown**)&pEnum));

    VARIANT nextElt;
    ULONG   nRead = 0;
    ::VariantInit (&nextElt);
    for (; SUCCEEDED (pEnum->Next (1, &nextElt, &nRead)) and nRead == 1;) {
        INetFwRule* r = nullptr;
        ThrowIfErrorHRESULT (nextElt.punkVal->QueryInterface (&r));
        rules += ReadRule_ (r);
        r->Release ();
    }
#endif
    return move (rules);
}
