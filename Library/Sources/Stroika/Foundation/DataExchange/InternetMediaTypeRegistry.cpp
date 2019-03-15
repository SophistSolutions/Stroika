/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Characters/String_Constant.h"
#include "../Characters/ToString.h"
#include "../Containers/Bijection.h"
#include "../DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#if qPlatform_Windows
#include "../Configuration/Platform/Windows/Registry.h"
#endif
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

/*
 ********************************************************************************
 *************************** InternetMediaTypeRegistry **************************
 ********************************************************************************
 */
optional<FileSuffixType> InternetMediaTypeRegistry::GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const
{
#if qPlatform_Windows
    if (auto fs = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.LookupPref (Characters::Format (L"MIME\\Database\\Content Type\\%s\\Extension", ct.As<String> ().c_str ()))) {
        return fs.As<String> ();
    }
#else
    AssertNotImplemented ();
#endif
    return nullopt;
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

optional<String> InternetMediaTypeRegistry::GetAssociatedPrettyName (const InternetMediaType& ct) const
{
#if qPlatform_Windows
    if (optional<FileSuffixType> fileSuffix = GetPreferredAssociatedFileSuffix (ct)) {
        if (auto fileTypeID = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.LookupPref (*fileSuffix)) {
            if (auto prettyName = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.LookupPref (fileTypeID.As<String> ())) {
                return prettyName.As<String> ();
            }
        }
    }
#else
    AssertNotImplemented ();
#endif
    return nullopt;
}

optional<InternetMediaType> InternetMediaTypeRegistry::GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const
{
    FileSuffixType suffix = IO::FileSystem::GetFileSuffix (fileNameOrSuffix);
#if qPlatform_Linux
    return GetGlobsFile_ ().Lookup (suffix);
#elif qPlatform_Windows
    using Characters::Format;
    using Configuration::Platform::Windows::RegistryKey;
    if (auto oct = RegistryKey{HKEY_CLASSES_ROOT}.LookupPref (Format (L"%s\\Content Type", suffix.c_str ()))) {
        return InternetMediaType{oct.As<String> ()};
    }
#else
    AssertNotImplemented ();
#endif
    return {};
}