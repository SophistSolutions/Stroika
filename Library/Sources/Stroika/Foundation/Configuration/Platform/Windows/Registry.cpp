/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Execution/DLLSupport.h"
#include "../../../Execution/Platform/Windows/Exception.h"

#include "../../../Memory/StackBuffer.h"

#include "./Registry.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Configuration::Platform::Windows;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution::Platform::Windows;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************************* RegistryKey **********************************
 ********************************************************************************
 */
RegistryKey::RegistryKey (HKEY parentKey, const String& path, REGSAM samDesired)
    : fKey_{OpenPath_ (parentKey, path, samDesired)}
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

namespace {
    // missingReturnsEmpty flag because I happen to have defined the semantics for Lookup () that way, and its reasonable
    // so lets not change now... --LGP 2020-07-05
    VariantValue ExtractValue_ (HKEY key, const TCHAR* path, bool missingReturnsEmpty)
    {
        DWORD dwType         = 0;
        DWORD dwCountInBytes = 0;
        LONG  lResult        = ::RegQueryValueExW (key, path, nullptr, &dwType, nullptr, &dwCountInBytes);
        if (lResult == ERROR_SUCCESS) {
            switch (dwType) {
                case REG_SZ:
                case REG_EXPAND_SZ: {
                    wstring strValue;
                    if (dwCountInBytes != 0) {
                        Assert (dwCountInBytes % sizeof (wchar_t) == 0); // @todo - we should bullet proof this code more but for now, assert if an issue
                        size_t nChars = dwCountInBytes / 2 - 1;          // includes NUL-byte
                        strValue.resize (nChars);
                        ThrowIfNotERROR_SUCCESS (::RegQueryValueExW (key, path, nullptr, &dwType, (LPBYTE) & (*strValue.begin ()), &dwCountInBytes));
                        Assert (strValue[nChars] == '\0');
                    }
                    return VariantValue{strValue};
                } break;
                case REG_DWORD: {
                    DWORD result{};
                    Assert (dwCountInBytes == sizeof (DWORD));
                    ThrowIfNotERROR_SUCCESS (::RegQueryValueExW (key, path, nullptr, &dwType, (LPBYTE)&result, &dwCountInBytes));
                    Assert (dwCountInBytes == sizeof (DWORD));
                    return VariantValue{result};
                } break;
                default: {
                    WeakAssert (false); // NYI
                    Execution::Throw (Execution::Exception<> (L"Unsupported registry format"sv));
                } break;
            }
        }
        // Just treat this as an error and revisit later if we get issues and maybe change / document behavior for Lookup()
        else if (lResult == ERROR_FILE_NOT_FOUND and missingReturnsEmpty) {
            return VariantValue{}; // @todo reconsider if we should throw here or not??? -- LGP 2020-07-04
        }
        else {
            Execution::Throw (Execution::SystemErrorException<> (lResult, system_category ()));
        }
    }
}

String RegistryKey::GetFullPathOfKey () const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper trcCtx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}::RegistryKey::GetFullPathOfKey")};
#endif
    // Based on https://stackoverflow.com/questions/937044/determine-path-to-registry-key-from-hkey-handle-in-c

    using NTSTATUS = LONG;
#ifndef STATUS_SUCCESS
    const auto STATUS_SUCCESS{((NTSTATUS)0x00000000L)};
#endif
#ifndef STATUS_BUFFER_TOO_SMALL
    constexpr auto STATUS_BUFFER_TOO_SMALL{(NTSTATUS)0xC0000023L};
#endif
    std::wstring keyPath;
    if (fKey_ != NULL) {
        Execution::DLLLoader dll{L"ntdll.dll"};
        using NtQueryKeyType = DWORD (__stdcall*) (HANDLE KeyHandle, int KeyInformationClass, PVOID KeyInformation, ULONG Length, PULONG ResultLength);
        NtQueryKeyType func  = reinterpret_cast<NtQueryKeyType> (dll.GetProcAddress ("NtQueryKey"));
        DWORD          size{0};
        DWORD          result = func (fKey_, 3, 0, 0, &size);
        if (result == STATUS_BUFFER_TOO_SMALL) {
            size            = size + 2;
            wchar_t* buffer = new (std::nothrow) wchar_t[size / sizeof (wchar_t)]; // size is in bytes
            if (buffer != NULL) {
                result = func (fKey_, 3, buffer, size, &size);
                if (result == STATUS_SUCCESS) {
                    buffer[size / sizeof (wchar_t)] = L'\0';
                    keyPath                         = std::wstring (buffer + 2);
                }
                delete[] buffer;
            }
        }
    }
    return keyPath;
}

VariantValue RegistryKey::Lookup (const String& valuePath) const
{
    Require (fKey_ != INVALID_HANDLE_VALUE);
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
    return ExtractValue_ (fKey_, valuePath.c_str (), true);
}

Traversal::Iterable<shared_ptr<RegistryKey>> RegistryKey::EnumerateSubKeys () const
{
    Require (fKey_ != INVALID_HANDLE_VALUE);

    // Use a generator to avoid keeping tons of registry key objects in memory at the same time (in case this is a limited resource)
    struct Context_ {
        HKEY fParentKey;
        int  fCurIndex{0};
    };
    auto myContext        = make_shared<Context_> ();
    myContext->fParentKey = fKey_;
    auto getNext          = [myContext] () -> optional<shared_ptr<RegistryKey>> {
        Memory::StackBuffer<TCHAR> achKeyBuf{Memory::eUninitialized, 1024};
        DWORD                      cbName = static_cast<DWORD> (achKeyBuf.size ()); // size of name string
    retry:
        auto retCode = ::RegEnumKeyEx (myContext->fParentKey, myContext->fCurIndex, achKeyBuf.begin (), &cbName, nullptr, nullptr, nullptr, nullptr);
        if (retCode == ERROR_NO_MORE_ITEMS) {
            return nullopt; // done
        }
        if (retCode == ERROR_MORE_DATA) {
            achKeyBuf.GrowToSize (achKeyBuf.size () * 2);
            goto retry;
        }
        ThrowIfNotERROR_SUCCESS (retCode);
        myContext->fCurIndex++;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"returning next child key: %s", String::FromSDKString (achKeyBuf).c_str ());
#endif
        return make_shared<RegistryKey> (myContext->fParentKey, String::FromSDKString (achKeyBuf));
    };
    return Traversal::CreateGenerator<shared_ptr<RegistryKey>> (getNext);
}

Containers::Mapping<Characters::String, DataExchange::VariantValue> RegistryKey::EnumerateValues () const
{
    Containers::Mapping<Characters::String, DataExchange::VariantValue> result;
    for (int i = 0;; ++i) {
        Memory::StackBuffer<TCHAR> achKeyBuf{Memory::eUninitialized, 1024};
        DWORD                      cbName = static_cast<DWORD> (achKeyBuf.size ()); // size of name string
    retry:
        auto retCode = ::RegEnumValue (fKey_, i, achKeyBuf.begin (), &cbName, nullptr, nullptr, nullptr, nullptr);
        if (retCode == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (retCode == ERROR_MORE_DATA) {
            achKeyBuf.GrowToSize (achKeyBuf.size () * 2);
            goto retry;
        }
        ThrowIfNotERROR_SUCCESS (retCode);
        result.Add (String::FromSDKString (achKeyBuf), ExtractValue_ (fKey_, achKeyBuf.begin (), false));
    }
    return result;
}
