/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Characters/String_Constant.h"
#include "../Characters/ToString.h"
#include "../Containers/Bijection.h"
#include "../DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "../Debug/Trace.h"
#if qPlatform_Windows
#include "../Execution/Platform/Windows/Exception.h"
#endif
#include "../IO/FileSystem/FileInputStream.h"
#include "../IO/FileSystem/PathName.h"

#include "InternetMediaTypeRegistry.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using FileSuffixType = InternetMediaTypeRegistry::FileSuffixType;

#if qPlatform_Windows
using Stroika::Foundation::Execution::Platform::Windows::ThrowIfNotERROR_SUCCESS;
#endif

#if qPlatform_Linux
// not sure what platforms have this
namespace {
    Mapping<FileSuffixType, InternetMediaType> GetGlobsFile_ ()
    {
        auto readGlobsFile = []() -> Mapping<FileSuffixType, InternetMediaType> {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"{}readGlobsFile"};
#endif
            Mapping<FileSuffixType, InternetMediaType> results{};
            for (Sequence<String> line : DataExchange::Variant::CharacterDelimitedLines::Reader{{':'}}.ReadMatrix (IO::FileSystem::FileInputStream::New (L"/usr/share/mime/globs"))) {
                if (line.length () == 2) {
                    String glob = line[1];
                    if (glob.StartsWith ('*')) {
                        glob = glob.SubString (1);
                    }
                    results.Add (glob, InternetMediaType{line[0]});
                }
            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"returning %s", Characters::ToString (results).c_str ());
#endif
            return results;
        };
        static const Mapping<FileSuffixType, InternetMediaType> kData_ = readGlobsFile ();
        return kData_;
    }
}
#endif

#if qPlatform_Windows
namespace {
    // Based on code from HealthFrame. MAYBE someday do more generic version of this - as helper
    // @see https://stroika.atlassian.net/browse/STK-575
    struct OptionsFileHelper_ {
        OptionsFileHelper_ (HKEY hkey)
            : fKey (hkey)
        {
            Assert (nullptr != INVALID_HANDLE_VALUE);
            if (hkey == INVALID_HANDLE_VALUE or hkey == nullptr) {
                Execution::Throw (Execution::Platform::Windows::Exception (ERROR_INVALID_HANDLE), "OptionsFileHelper::CTOR - bad handle");
            }
        }
        OptionsFileHelper_ (HKEY parentKey, const String& path, REGSAM samDesired = KEY_READ | KEY_WRITE)
            : fKey (OpenWithCreateAlongPath (parentKey, path, samDesired))
        {
            if (parentKey == INVALID_HANDLE_VALUE or parentKey == nullptr) {
                Execution::Throw (Execution::Platform::Windows::Exception (ERROR_INVALID_HANDLE), "OptionsFileHelper::CTOR - bad handle");
            }
        }
        OptionsFileHelper_ ()                          = delete;
        OptionsFileHelper_ (const OptionsFileHelper_&) = delete;
        const OptionsFileHelper_& operator= (const OptionsFileHelper_&) const = delete;
        ~OptionsFileHelper_ ()
        {
            Assert (nullptr != INVALID_HANDLE_VALUE);
            Assert (fKey != nullptr);
            Assert (fKey != INVALID_HANDLE_VALUE);
            ::RegCloseKey (fKey);
        }
        /*
         * Walk the given path (in segments) - and make sure each exists, and create each segment if it doesn't
         * exist.Finally - do a regular registry open with access permissions 'samDesired'.< / p>
         */
        static HKEY OpenWithCreateAlongPath (HKEY parentKey, const String& path, REGSAM samDesired = KEY_READ | KEY_WRITE)
        {
            Debug::TraceContextBumper trcCtx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"OptionsFileHelper::OpenWithCreateAlongPath", L"parentKey=%p, path='%s'", parentKey, path.c_str ())};
            Require (parentKey != nullptr);
            size_t prevPos = 0;
            HKEY   curPar  = parentKey;
            for (;;) {
                size_t    endPos    = path.find ('\\', prevPos);
                SDKString segName   = path.AsSDKString ().substr (prevPos, endPos == SDKString::npos ? endPos : (endPos - prevPos));
                REGSAM    createSAM = (endPos == SDKString::npos) ? samDesired : KEY_READ;
                HKEY      newKey    = static_cast<HKEY> (INVALID_HANDLE_VALUE);

                ThrowIfNotERROR_SUCCESS (::RegCreateKeyEx (curPar, segName.c_str (), 0, REG_NONE, REG_OPTION_NON_VOLATILE, createSAM, nullptr, &newKey, nullptr));
                if (curPar != parentKey) {
                    ::RegCloseKey (curPar);
                }
                curPar = newKey;
                if (endPos == SDKString::npos) {
                    return newKey;
                }
                prevPos = endPos + 1;
            }
        }

        nonvirtual bool LookupPref (const String& prefName, string* value, DWORD* regStrType = nullptr) const
        {
            RequireNotNull (value);
            Assert (fKey != INVALID_HANDLE_VALUE);
            string strValue;
            DWORD  dwType  = 0;
            DWORD  dwCount = 0;
            LONG   lResult = ::RegQueryValueExA (fKey, prefName.AsNarrowSDKString ().c_str (), nullptr, &dwType, nullptr, &dwCount);
            if (lResult == ERROR_SUCCESS) {
                if (dwType == REG_SZ or dwType == REG_EXPAND_SZ) {
                    if (dwCount != 0) {
                        strValue.resize (dwCount);
                        lResult = ::RegQueryValueExA (fKey, prefName.AsNarrowSDKString ().c_str (), nullptr, &dwType, (LPBYTE) & (*strValue.begin ()), &dwCount);
                    }
                }
                else {
                    lResult = !ERROR_SUCCESS;
                }
            }
            if (lResult == ERROR_SUCCESS) {
                if (dwType == REG_SZ or dwType == REG_EXPAND_SZ) {
                    *value = strValue.c_str (); // copying like this loses xtra NUL-byte if there is one from read...
                    if (regStrType != nullptr) {
                        *regStrType = dwType;
                    }
                    return true;
                }
            }
            return false;
        }
        nonvirtual bool LookupPref (const String& prefName, wstring* value, DWORD* regStrType = nullptr) const
        {
            RequireNotNull (value);
            Assert (fKey != INVALID_HANDLE_VALUE);
            wstring strValue;
            DWORD   dwType  = 0;
            DWORD   dwCount = 0;
            LONG    lResult = ::RegQueryValueExW (fKey, prefName.c_str (), nullptr, &dwType, nullptr, &dwCount);
            if (lResult == ERROR_SUCCESS) {
                if (dwType == REG_SZ or dwType == REG_EXPAND_SZ) {
                    if (dwCount != 0) {
                        strValue.resize (dwCount);
                        lResult = ::RegQueryValueExW (fKey, prefName.c_str (), nullptr, &dwType, (LPBYTE) & (*strValue.begin ()), &dwCount);
                    }
                }
                else {
                    lResult = !ERROR_SUCCESS;
                }
            }
            if (lResult == ERROR_SUCCESS) {
                if (dwType == REG_SZ or dwType == REG_EXPAND_SZ) {
                    *value = strValue.c_str (); // copying like this loses xtra NUL-byte if there is one from read...
                    if (regStrType != nullptr) {
                        *regStrType = dwType;
                    }
                    return true;
                }
            }
            // If not success - then maybe cuz non-UNICODE functions not available - so try to read a non-UNICODE string
            {
                string tmp;
                if (LookupPref (prefName, &tmp, regStrType)) {
                    *value = NarrowSDKStringToWide (tmp);
                    return true;
                }
            }
            return false;
        }

    private:
        HKEY fKey;
    };
    Optional<String> GrabRegistryStringValue_ (HKEY parentKey, const String& path)
    {
        try {
            String eltPath = path;
            String parentPath;
            {
                size_t i = path.rfind ('\\');
                if (i != SDKString::npos) {
                    parentPath = path.substr (0, i);
                    eltPath    = path.substr (i + 1);
                }
            }
            OptionsFileHelper_ ofh (parentKey, parentPath, KEY_READ);
            wstring            result;
            if (ofh.LookupPref (eltPath, &result)) {
                return result;
            }
        }
        catch (...) {
            // ignore
        }
        return {};
    }
}
#endif
/*
 ********************************************************************************
 *************************** InternetMediaTypeRegistry **************************
 ********************************************************************************
 */

Optional<FileSuffixType> InternetMediaTypeRegistry::GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const
{
#if qPlatform_Windows
    return GrabRegistryStringValue_ (HKEY_CLASSES_ROOT, Characters::Format (L"MIME\\Database\\Content Type\\%s\\Extension", ct.As<String> ().c_str ()));
#else
    AssertNotImplemented ();
    return {};
#endif
}

Sequence<String> InternetMediaTypeRegistry::GetAssociatedFileSuffixes (const InternetMediaType& ct, bool includeMoreGeneralTypes) const
{
#if qPlatform_Windows
    Sequence<String> tmp;
    // @todo - hack - inadequate, but good enough to start
    if (auto i = GetPreferredAssociatedFileSuffix (ct)) {
        tmp += *i;
    }
    return tmp;
#else
    AssertNotImplemented ();
    return {};
#endif
}

Optional<String> InternetMediaTypeRegistry::GetAssociatedPrettyName (const InternetMediaType& ct) const
{
#if qPlatform_Windows
    if (Optional<FileSuffixType> fileSuffix = GetPreferredAssociatedFileSuffix (ct)) {
        if (Optional<String> fileTypeID = GrabRegistryStringValue_ (HKEY_CLASSES_ROOT, *fileSuffix)) {
            return GrabRegistryStringValue_ (HKEY_CLASSES_ROOT, *fileTypeID);
        }
    }
#else
    AssertNotImplemented ();
#endif
    return {};
}

Optional<InternetMediaType> InternetMediaTypeRegistry::GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const
{
    FileSuffixType suffix = IO::FileSystem::GetFileSuffix (fileNameOrSuffix);
#if qPlatform_Linux
    return GetGlobsFile_ ().Lookup (suffix);
#elif qPlatform_Windows
    if (Optional<String> oct = GrabRegistryStringValue_ (HKEY_CLASSES_ROOT, Characters::Format (L"%s\\Content Type", suffix.c_str ()))) {
        return InternetMediaType{*oct};
    }
#else
    AssertNotImplemented ();
#endif
    return {};
}