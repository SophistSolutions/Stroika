/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
    sb += L"{";
    sb += L"fName: " + Characters::ToString (fName) + L",";
    sb += L"fDescription: " + Characters::ToString (fDescription) + L",";
    sb += L"fApplication: " + Characters::ToString (fApplication) + L",";
#if qPlatform_Windows
    sb += L"fProfileMask: " + Characters::ToString ((int)fProfileMask) + L",";
    sb += L"fDirection: " + Characters::ToString ((int)fDirection) + L",";
    sb += L"fProtocol: " + Characters::ToString ((int)fProtocol) + L",";
#endif
    sb += L"fLocalPorts: " + Characters::ToString (fLocalPorts) + L",";
    sb += L"fRemotePorts: " + Characters::ToString (fRemotePorts) + L",";
#if qPlatform_Windows
    sb += L"fAction: " + Characters::ToString ((int)fAction) + L",";
#endif
    sb += L"fEnabled: " + Characters::ToString (fEnabled) + L",";
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ***************** IO::Network::SystemFirewall::Manager *************************
 ********************************************************************************
 */
SystemFirewall::Manager SystemFirewall::Manager::sThe;

#if qPlatform_Windows
namespace {
    Rule ReadRule_ (INetFwRule* pFwRule)
    {
        AssertNotNull (pFwRule);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"{}ReadRule_"};
#endif
        BSTR name = nullptr;
        BSTR desc = nullptr;
        BSTR group = nullptr;
        BSTR application = nullptr;
        BSTR localPorts  = nullptr;
        BSTR remotePorts = nullptr;
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([=] () noexcept {
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
        return Rule{
            name == nullptr ? wstring () : wstring (name),
            desc == nullptr ? wstring () : wstring (desc),
            group == nullptr ? wstring () : wstring (group),
            application == nullptr ? wstring () : wstring (application),
            (NET_FW_PROFILE_TYPE2) (profileMask),
            direction,
            (NET_FW_IP_PROTOCOL_) (protocol),
            localPorts == nullptr ? wstring () : wstring (localPorts),
            remotePorts == nullptr ? wstring () : wstring (remotePorts),
            action,
            enabled != VARIANT_FALSE};
    }
    optional<Rule> ReadRule_ (INetFwRules* pFwRules, const String& ruleName)
    {
        AssertNotNull (pFwRules);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"{}ReadRule_"};
#endif
        INetFwRule*             pFwRule           = nullptr;
        [[maybe_unused]] auto&& cleanupCOMObjects = Execution::Finally ([=] () noexcept {
            if (pFwRule != nullptr) {
                pFwRule->Release ();
            }
        });
        HRESULT                 hr                = pFwRules->Item (SmartBSTR{ruleName.c_str ()}, &pFwRule);
        if (hr == S_OK and pFwRule != nullptr) {
            return ReadRule_ (pFwRule);
        }
        return nullopt;
    }
}
#endif

bool SystemFirewall::Manager::Register (const Rule& rule)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"SystemFirewall::Manager::Register", L"rule=%s", Characters::ToString(rule).c_str ())};

    for (auto r : LookupByGroup (rule.fGroup)) {
        if (r == rule) {
            DbgTrace ("run unchanged, so returning false");
            return false;
        }
    }

#if qPlatform_Windows
    // Initialize COM
    HRESULT                 hrComInit = CoInitializeEx (0, COINIT_APARTMENTTHREADED);
    [[maybe_unused]] auto&& cleanupCO = Execution::Finally ([hrComInit] () noexcept {
        if (SUCCEEDED (hrComInit)) {
            CoUninitialize ();
        }
    });

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        ThrowIfErrorHRESULT (hrComInit);
    }

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
    ThrowIfErrorHRESULT (CoCreateInstance (__uuidof(NetFwPolicy2), nullptr, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2));

    // Retrieve INetFwRules
    ThrowIfErrorHRESULT (pNetFwPolicy2->get_Rules (&pFwRules));

    #if 0
    if (auto o = ReadRule_ (pFwRules, rule.fName)) {
        if (*o == rule) {
            DbgTrace ("run unchanged, so returning false");
            return false;
        }
    }
    #endif

    // Create a new Firewall Rule object.
    ThrowIfErrorHRESULT (CoCreateInstance (__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule));

    // Populate the Firewall Rule object
    ThrowIfErrorHRESULT (pFwRule->put_Name (SmartBSTR{rule.fName.c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_Description (SmartBSTR{rule.fDescription.c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_ApplicationName (SmartBSTR{rule.fApplication.c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_Protocol (rule.fProtocol));
    ThrowIfErrorHRESULT (pFwRule->put_LocalPorts (SmartBSTR{rule.fLocalPorts.c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_RemotePorts (SmartBSTR{rule.fRemotePorts.c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_Direction (rule.fDirection));
    ThrowIfErrorHRESULT (pFwRule->put_Grouping (SmartBSTR{rule.fGroup.c_str ()}));
    ThrowIfErrorHRESULT (pFwRule->put_Profiles (rule.fProfileMask));
    ThrowIfErrorHRESULT (pFwRule->put_Action (rule.fAction));
    ThrowIfErrorHRESULT (pFwRule->put_Enabled (rule.fEnabled ? VARIANT_TRUE : VARIANT_FALSE));

    // Add the Firewall Rule
    DbgTrace ("Updating firewall rule");
    ThrowIfErrorHRESULT (pFwRules->Add (pFwRule));
#endif
    return true;
}

optional<Rule> SystemFirewall::Manager::Lookup (const String& ruleName)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"SystemFirewall::Manager::Lookup", L"ruleName=%s", ruleName.c_str ())};
#if qPlatform_Windows
    // Initialize COM.
    HRESULT                 hrComInit = CoInitializeEx (0, COINIT_APARTMENTTHREADED);
    [[maybe_unused]] auto&& cleanupCO = Execution::Finally ([hrComInit] () noexcept {
        if (SUCCEEDED (hrComInit)) {
            CoUninitialize ();
        }
    });

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        ThrowIfErrorHRESULT (hrComInit);
    }

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
    ThrowIfErrorHRESULT (CoCreateInstance (__uuidof(NetFwPolicy2), nullptr, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2));

    // Retrieve INetFwRules
    ThrowIfErrorHRESULT (pNetFwPolicy2->get_Rules (&pFwRules));
    return ReadRule_ (pFwRules, ruleName);
#endif
    return nullopt;
}

Traversal::Iterable<Rule> SystemFirewall::Manager::LookupByGroup (const String& groupName)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"SystemFirewall::Manager::LookupByGroup", L"groupName=%s", groupName.c_str ())};
    Collection<Rule>          rules;
    for (Rule r : LookupAll ()) {
        if (r.fGroup == groupName) {
            rules += r;
        }
    }
    return rules;
}

Traversal::Iterable<Rule> SystemFirewall::Manager::LookupAll ()
{
    Debug::TraceContextBumper ctx{L"SystemFirewall::Manager::LookupAll"};
    Collection<Rule>          rules;
#if qPlatform_Windows
    // Initialize COM.
    HRESULT                 hrComInit = CoInitializeEx (0, COINIT_APARTMENTTHREADED);
    [[maybe_unused]] auto&& cleanupCO = Execution::Finally ([hrComInit] () noexcept {
        if (SUCCEEDED (hrComInit)) {
            CoUninitialize ();
        }
    });

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        ThrowIfErrorHRESULT (hrComInit);
    }

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
    ThrowIfErrorHRESULT (CoCreateInstance (__uuidof(NetFwPolicy2), nullptr, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2));

    // Retrieve INetFwRules
    ThrowIfErrorHRESULT (pNetFwPolicy2->get_Rules (&pFwRules));

    ThrowIfErrorHRESULT (pFwRules->get__NewEnum ((IUnknown**)&pEnum));

    VARIANT nextElt;
    ULONG   nRead = 0;
    ::VariantInit (&nextElt);
    for (; SUCCEEDED (pEnum->Next (1, &nextElt, &nRead)) and nRead == 1;) {
        INetFwRule* r = nullptr;
        ThrowIfErrorHRESULT ((*nextElt.ppunkVal)->QueryInterface (&r));
        rules += ReadRule_ (r);
        r->Release ();
    }
#endif
    return rules;
}
