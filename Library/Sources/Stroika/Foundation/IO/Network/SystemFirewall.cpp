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
#if qPlatform_Windows
using namespace Stroika::Foundation::Execution::Platform::Windows;
#endif
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::SystemFirewall;

#if qPlatform_Windows
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#endif

/*
 ********************************************************************************
 ***************** IO::Network::SystemFirewall::Manager *************************
 ********************************************************************************
 */
SystemFirewall::Manager SystemFirewall::Manager::sThe;

bool SystemFirewall::Manager::Register (const Rule& rule)
{
    Debug::TraceContextBumper ctx{L"SystemFirewall::Manager::Register"};

#if qPlatform_Windows
    long CurrentProfilesBitMask = 0;

    SmartBSTR bstrRuleName{rule.fName.c_str ()};
    SmartBSTR bstrRuleDescription{rule.fDescription.c_str ()};
    SmartBSTR bstrRuleGroup{rule.fGroup.c_str ()};
    SmartBSTR bstrRuleApplication{rule.fApplication.c_str ()};
    SmartBSTR bstrRuleLPorts{rule.fPorts.c_str ()};

    // Initialize COM.
    HRESULT                 hrComInit = CoInitializeEx (0, COINIT_APARTMENTTHREADED);
    [[maybe_unused]] auto&& cleanupCO = Execution::Finally ([hrComInit] () noexcept { if (SUCCEEDED (hrComInit)) {
        CoUninitialize ();
    } });

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

    {
        HRESULT hr = pFwRules->Item (bstrRuleName, &pFwRule);
        if (hr == S_OK and pFwRule != nullptr) {
            BSTR desc = nullptr;
            pFwRule->get_Description (&desc);
            BSTR group = nullptr;
            pFwRule->get_Grouping (&group);
            bool isSame = true;
            if (wstring (desc) != rule.fDescription.As<wstring> ()) {
                isSame = false;
            }
            if (wstring (group) != rule.fGroup.As<wstring> ()) {
                isSame = false;
            }
            if (isSame) {
                DbgTrace ("they are the same");
                return false;
            }
        }
    }

    // Retrieve Current Profiles bitmask
    ThrowIfErrorHRESULT (pNetFwPolicy2->get_CurrentProfileTypes (&CurrentProfilesBitMask));

    // Create a new Firewall Rule object.
    ThrowIfErrorHRESULT (CoCreateInstance (__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule));

    // Populate the Firewall Rule object
    pFwRule->put_Name (bstrRuleName);
    pFwRule->put_Description (bstrRuleDescription);
    pFwRule->put_ApplicationName (bstrRuleApplication);
    pFwRule->put_Protocol (rule.fProtocol);
    pFwRule->put_LocalPorts (bstrRuleLPorts);
    pFwRule->put_Direction (rule.fDirection);
    pFwRule->put_Grouping (bstrRuleGroup);
    pFwRule->put_Profiles (rule.fProfileMask);
    pFwRule->put_Action (rule.fAction);
    pFwRule->put_Enabled (rule.fEnabled ? VARIANT_TRUE : VARIANT_FALSE);

    // Add the Firewall Rule
    ThrowIfErrorHRESULT (pFwRules->Add (pFwRule));
#endif
    return true;
}

optional<Rule> SystemFirewall::Manager::Lookup (const String& ruleName)
{
    return nullopt;
}
