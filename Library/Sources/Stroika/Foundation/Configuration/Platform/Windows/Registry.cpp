/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif
#include "../../../Execution/Platform/Windows/Exception.h"

#include "./Registry.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Configuration::Platform::Windows;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution::Platform::Windows;


/*
 ********************************************************************************
 ********************************* RegistryKey **********************************
 ********************************************************************************
 */
RegistryKey::RegistryKey (HKEY parentKey, const String& path, REGSAM samDesired)
	: fKey_ (OpenWithCreateAlongPath_ (parentKey, path, samDesired))
	, fOwned_{ true }
{
}

/*
 * Walk the given path (in segments) - and make sure each exists, and create each segment if it doesn't
 * exist.Finally - do a regular registry open with access permissions 'samDesired'.< / p>
 */
 HKEY RegistryKey::OpenWithCreateAlongPath_ (HKEY parentKey, const String& path, REGSAM samDesired)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
	Debug::TraceContextBumper trcCtx{ Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}::OptionsFileHelper_::OpenWithCreateAlongPath", L"parentKey=%p, path='%s'", parentKey, path.c_str ()) };
#endif
	Require (parentKey != nullptr);
	Require (parentKey != INVALID_HANDLE_VALUE);
	Require (samDesired == KEY_READ);			// for now - later allow others so long as they are non-destructive/readonly
	HKEY   result{  };
	ThrowIfNotERROR_SUCCESS (::RegOpenKeyEx (parentKey, path.AsSDKString ().c_str (), 0, samDesired, &result));
	Ensure (result != nullptr and result != INVALID_HANDLE_VALUE);
	return result;
}

 VariantValue RegistryKey::LookupPref (const String& prefName) const
{
	Assert (fKey_ != INVALID_HANDLE_VALUE);
	wstring strValue;
	DWORD   dwType = 0;
	DWORD   dwCount = 0;
	LONG    lResult = ::RegQueryValueExW (fKey_, prefName.c_str (), nullptr, &dwType, nullptr, &dwCount);
	if (lResult == ERROR_SUCCESS) {
		if (dwType == REG_SZ or dwType == REG_EXPAND_SZ) {
			if (dwCount != 0) {
				strValue.resize (dwCount);
				lResult = ::RegQueryValueExW (fKey_, prefName.c_str (), nullptr, &dwType, (LPBYTE) & (*strValue.begin ()), &dwCount);
			}
		}
		else {
			lResult = !ERROR_SUCCESS;
		}
	}
	if (lResult == ERROR_SUCCESS) {
		switch (dwType) {
		case REG_SZ:
		case REG_EXPAND_SZ:
			return VariantValue{ strValue };
		case REG_DWORD:
			// todo - and more cases...
			;
		}
		AssertNotImplemented ();	// must support reading other types!!!
	}
	return VariantValue{};
}
