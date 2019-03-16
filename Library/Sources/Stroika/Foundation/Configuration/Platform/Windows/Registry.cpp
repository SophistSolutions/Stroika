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
    : fKey_ (OpenPath_ (parentKey, path, samDesired))
    , fOwned_{true}
{
}

/*
 * Walk the given path (in segments) - and make sure each exists, and create each segment if it doesn't
 * exist.Finally - do a regular registry open with access permissions 'samDesired'.< / p>
 */
HKEY RegistryKey::OpenPath_ (HKEY parentKey, const String& path, REGSAM samDesired)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper trcCtx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}::RegistryKey::OpenPath_", L"parentKey=%p, path='%s'", parentKey, path.c_str ())};
#endif
    Require (parentKey != nullptr);
    Require (parentKey != INVALID_HANDLE_VALUE);
    Require (samDesired == KEY_READ); // @todo - for now - later allow others so long as they are non-destructive/readonly
    HKEY result{};
    ThrowIfNotERROR_SUCCESS (::RegOpenKeyEx (parentKey, path.AsSDKString ().c_str (), 0, samDesired, &result));
    Ensure (result != nullptr and result != INVALID_HANDLE_VALUE);
    return result;
}

VariantValue RegistryKey::Lookup (const String& valuePath) const
{
    Assert (fKey_ != INVALID_HANDLE_VALUE);

    {
        // RegQueryValueExW doesn't support this directly, but its quite handy, and we document we support this
        // (allowing path syntax in the valuePath to lookup)
        size_t lastBackSlash = valuePath.rfind ('\\');
        if (lastBackSlash != SDKString::npos) {
            // @todo - check on TYPE of exception and if cuz not there, return empty, and if cuz of permissions (etc)
            // pass along exception (note https://docs.microsoft.com/en-us/windows/desktop/api/winreg/nf-winreg-regopenkeyexa doesnt document returned error codes for not found)
            try {
                return RegistryKey{fKey_, valuePath.substr (0, lastBackSlash)}.Lookup (valuePath.substr (lastBackSlash + 1));
            }
            catch (const system_error& e) {
                // catch/translate because the part not found could be in the PATH and then RegistryKey would throw
                if (e.code () == errc::no_such_file_or_directory) { // windows error ERROR_FILE_NOT_FOUND
                    return VariantValue{};
                }
                Execution::ReThrow ();
            }
        }
    }

    DWORD dwType         = 0;
    DWORD dwCountInBytes = 0;
    LONG  lResult        = ::RegQueryValueExW (fKey_, valuePath.c_str (), nullptr, &dwType, nullptr, &dwCountInBytes);
    if (lResult == ERROR_SUCCESS) {
        switch (dwType) {
            case REG_SZ:
            case REG_EXPAND_SZ: {
                wstring strValue;
                if (dwCountInBytes != 0) {
                    Assert (dwCountInBytes % sizeof (wchar_t) == 0); // @todo - we should bullet proof this code more but for now, assert if an issue
                    size_t nChars = dwCountInBytes / 2 - 1;          // includes NUL-byte
                    strValue.resize (nChars);
                    ThrowIfNotERROR_SUCCESS (::RegQueryValueExW (fKey_, valuePath.c_str (), nullptr, &dwType, (LPBYTE) & (*strValue.begin ()), &dwCountInBytes));
                    Assert (strValue[nChars] == '\0');
                }
                return VariantValue{strValue};
            } break;
            case REG_DWORD: {
                DWORD result{};
                Assert (dwCountInBytes == sizeof (DWORD));
                ThrowIfNotERROR_SUCCESS (::RegQueryValueExW (fKey_, valuePath.c_str (), nullptr, &dwType, (LPBYTE)&result, &dwCountInBytes));
                Assert (dwCountInBytes == sizeof (DWORD));
                return VariantValue{result};
            } break;
            default: {
                WeakAssert (false); // NYI
                Execution::Throw (Execution::Exception<> (L"Unsupported registry format"sv));
            } break;
        }
    }
    else if (lResult == ERROR_FILE_NOT_FOUND) {
        return VariantValue{};
    }
    else {
        Execution::Throw (Execution::SystemErrorException<> (lResult, system_category ()));
    }
}
