/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#if qPlatform_Windows
#include <windows.h>
#endif

#include "Stroika/Foundation/Characters/Platform/Windows/SmartBSTR.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Logger.h"

#include "Firewall.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Characters::Platform::Windows;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Firewall;

SystemFirewall SystemFirewall::sThe;

#if qPlatform_Windows
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

namespace {
    HRESULT WFCOMInitialize_ (INetFwPolicy2** ppNetFwPolicy2)
    {
        HRESULT hr = S_OK;
        hr         = CoCreateInstance (__uuidof(NetFwPolicy2), nullptr, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)ppNetFwPolicy2);
        if (FAILED (hr)) {
            DbgTrace ("CoCreateInstance for INetFwPolicy2 failed: 0x%08lx\n", hr);
            goto Cleanup;
        }
    Cleanup:
        return hr;
    }
}
#endif

bool SystemFirewall::Register (const Rule& rule)
{
    Debug::TraceContextBumper ctx{L"SystemFirewall::Register"};

#if qPlatform_Windows
    HRESULT hrComInit = S_OK;
    HRESULT hr        = S_OK;

    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRules*   pFwRules      = NULL;
    INetFwRule*    pFwRule       = NULL;

    long CurrentProfilesBitMask = 0;

    SmartBSTR bstrRuleName{rule.fName.c_str ()};
    SmartBSTR bstrRuleDescription{rule.fDescription.c_str ()};
    SmartBSTR bstrRuleGroup{rule.fGroup.c_str ()};
    SmartBSTR bstrRuleApplication{rule.fApplication.c_str ()};
    SmartBSTR bstrRuleLPorts{rule.fPorts.c_str ()};

    // Initialize COM.
    hrComInit = CoInitializeEx (0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE) {
        if (FAILED (hrComInit)) {
            DbgTrace ("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize_ (&pNetFwPolicy2);
    if (FAILED (hr)) {
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules (&pFwRules);
    if (FAILED (hr)) {
        DbgTrace ("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    {
        hr = pFwRules->Item (bstrRuleName, &pFwRule);

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
    hr = pNetFwPolicy2->get_CurrentProfileTypes (&CurrentProfilesBitMask);
    if (FAILED (hr)) {
        DbgTrace ("get_CurrentProfileTypes failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Create a new Firewall Rule object.
    hr = CoCreateInstance (__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule);
    if (FAILED (hr)) {
        DbgTrace ("CoCreateInstance for Firewall Rule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

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
    hr = pFwRules->Add (pFwRule);
    if (FAILED (hr)) {
        DbgTrace ("Firewall Rule Add failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:
    // Release the INetFwRule object
    if (pFwRule != NULL) {
        pFwRule->Release ();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL) {
        pFwRules->Release ();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release ();
    }

    // Uninitialize COM.
    if (SUCCEEDED (hrComInit)) {
        CoUninitialize ();
    }
#endif
    return true;
}

optional<Rule> SystemFirewall::Lookup (String ruleName)
{
    return nullopt;
}
