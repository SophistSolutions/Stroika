/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Characters/String_Constant.h"
#include "../Characters/ToString.h"
#include "../Containers/Bijection.h"
#include "../DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "../DataExchange/XML/SAXReader.h"
#if qPlatform_Windows
#include "../Configuration/Platform/Windows/Registry.h"
#endif
#include "../Debug/Trace.h"
#if qPlatform_Windows
#include "../Execution/Platform/Windows/Exception.h"
#endif
#include "../Execution/Synchronized.h"
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

namespace {
    struct MIMEDB_ {
        // NOTE - we cannot use Bijection, because multiple media-types can map to a single filetype and not all mediatypes have a filetype
        Mapping<FileSuffixType, InternetMediaType> fSuffix2MediaTypeMap;
        Mapping<InternetMediaType, FileSuffixType> fMediaType2PreferredSuffixMap;
        Mapping<InternetMediaType, String>         fMediaType2PrettyName;

        MIMEDB_ ()
        {
#if qPlatform_POSIX
            // On POSIX systems, try these two locations to load MIME DB from files. DONT do on demand (per lookup) because you have to parse the entire files for these requests.
            if (LoadFromEtcMimeDotTypes ()) {
                return;
            }
            if (LoadFromEtcMimeDotTypes ()) {
                return;
            }
#endif
        }

        // return true if successful, and false (ignore) failure
        bool LoadFromUserShare ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"{}MIMEDB_::LoadFromUserShare"};
#endif
            try {
                for (Sequence<String> line : DataExchange::Variant::CharacterDelimitedLines::Reader{{':'}}.ReadMatrix (IO::FileSystem::FileInputStream::New (L"/usr/share/mime/globs"))) {
                    if (line.length () == 2) {
                        String glob = line[1];
                        if (glob.StartsWith ('*')) {
                            glob = glob.SubString (1);
                        }
                        fSuffix2MediaTypeMap.Add (glob, InternetMediaType{line[0]});
                        fMediaType2PreferredSuffixMap.Add (InternetMediaType{line[0]}, glob);
                    }
                }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"succeeded");
#endif
                return true;
            }
            catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"failure ignored");
#endif
                return false;
            }
        }

        // return true if successful, and false (ignore) failure
        bool LoadFromEtcMimeDotTypes ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"{}MIMEDB_::LoadFromEtcMimeDotTypes"};
#endif
            try {
                for (Sequence<String> line : DataExchange::Variant::CharacterDelimitedLines::Reader{{':'}}.ReadMatrix (IO::FileSystem::FileInputStream::New (L"/etc/mime.types"))) {
                    if (line.length () == 2 and not line[0].StartsWith (L"#")) {
                        InternetMediaType ct{line[0]};
                        String            suffix = line[1];
                        fSuffix2MediaTypeMap.Add (suffix, ct);
                        fMediaType2PreferredSuffixMap.Add (ct, suffix);
                    }
                }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"succeeded");
#endif
                return true;
            }
            catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"failure ignored");
#endif
                return false;
            }
        }

        optional<String> LookupAndUpdateFromUsrShareMimePrettyName (const InternetMediaType& ct)
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"{}MIMEDB_::LookupAndUpdateFromUsrShareMimePrettyName"};
#endif
            // SAX parse /usr/share/mime/TYPE/SUBTYPE.xml file and look for <comment> element (default with no language for now)
            // Simpler - just take the first - seems empirically fine/OK
            try {
                struct myHander_ : StructuredStreamEvents::IConsumer {
                    optional<String> fResult;
                    bool             onContentElt{false};
                    StringBuilder    fAccum;
                    virtual void     StartElement (const StructuredStreamEvents::Name& name) override
                    {
                        if (name == StructuredStreamEvents::Name{L"content"} and not fResult.has_value ()) {
                            onContentElt = true;
                        }
                    }
                    virtual void EndElement ([[maybe_unused]] const StructuredStreamEvents::Name& name) override
                    {
                        if (onContentElt) {
                            Assert (not fResult);
                            fResult = fAccum.str ();
                        }
                    }
                    virtual void TextInsideElement (const String& t) override
                    {
                        if (onContentElt) {
                            fAccum += t;
                        }
                    }
                };
                myHander_ handler;
                DataExchange::XML::SAXParse (IO::FileSystem::FileInputStream::New (L"/usr/share/mime/" + ct.GetType () + L"/" + ct.GetSubType () + L".xml"), handler);
                if (handler.fResult) {
                    fMediaType2PrettyName.Add (ct, *handler.fResult);
                    return *handler.fResult;
                }
            }
            catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"failure ignored");
#endif
            }
            return nullopt;
        }
    };

    RWSynchronized<MIMEDB_>& GetMIMEDB_ ()
    {
        static RWSynchronized<MIMEDB_> sPrivateDB_; // lazy construct on first call to usage
        return sPrivateDB_;
    }
}

/*
 ********************************************************************************
 *************************** InternetMediaTypeRegistry **************************
 ********************************************************************************
 */
optional<FileSuffixType> InternetMediaTypeRegistry::GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const
{
    if (auto o = GetMIMEDB_ ().cget ()->fMediaType2PreferredSuffixMap.Lookup (ct)) {
        return *o;
    }
#if qPlatform_Windows
    // only do registry lookup if needed, since (probably) more costly than local map lookup
    if (auto fs = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.Lookup (Characters::Format (L"MIME\\Database\\Content Type\\%s\\Extension", ct.As<String> ().c_str ()))) {
        GetMIMEDB_ ().rwget ()->fMediaType2PreferredSuffixMap.Add (ct, fs.As<String> ());
        return fs.As<String> ();
    }
#endif
    return nullopt;
}

Set<InternetMediaType> InternetMediaTypeRegistry::GetMoreGeneralTypes (const InternetMediaType& ct) const
{
    //@todo tmphack add others
    return Set<InternetMediaType>{ct};
}

Set<InternetMediaType> InternetMediaTypeRegistry::GetMoreSpecificTypes (const InternetMediaType& ct) const
{
    //@todo tmphack add others
    return Set<InternetMediaType>{ct};
}

Set<String> InternetMediaTypeRegistry::GetAssociatedFileSuffixes (const InternetMediaType& ct) const
{
    return GetAssociatedFileSuffixes (Iterable<InternetMediaType>{ct});
}

Set<String> InternetMediaTypeRegistry::GetAssociatedFileSuffixes (const Iterable<InternetMediaType>& cts) const
{
    Set<InternetMediaType> mediaTypes;
    cts.Apply ([&mediaTypes, this] (auto i) {
        mediaTypes += GetMoreGeneralTypes (i);
    });
    Set<String> result;
    for (auto ct : mediaTypes) {
        if (auto i = GetPreferredAssociatedFileSuffix (ct)) {
            result += *i;
        }
    }
    return result;
}

optional<String> InternetMediaTypeRegistry::GetAssociatedPrettyName (const InternetMediaType& ct) const
{
    if (auto o = GetMIMEDB_ ().cget ()->fMediaType2PrettyName.Lookup (ct)) {
        return *o;
    }
#if qPlatform_Windows
    if (optional<FileSuffixType> fileSuffix = GetPreferredAssociatedFileSuffix (ct)) {
        if (auto fileTypeID = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.Lookup (*fileSuffix + L"\\")) {
            if (auto prettyName = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.Lookup (fileTypeID.As<String> () + L"\\")) {
                return prettyName.As<String> ();
            }
        }
    }
#endif
#if qPlatform_POSIX
    if (auto o = GetMIMEDB_ ().rwget ()->LookupAndUpdateFromUsrShareMimePrettyName (ct)) {
        return *o;
    }
#endif
    return nullopt;
}

optional<InternetMediaType> InternetMediaTypeRegistry::GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const
{
    FileSuffixType suffix = IO::FileSystem::GetFileSuffix (fileNameOrSuffix);
    if (suffix.empty ()) {
        return nullopt;
    }
    Assert (suffix[0] == '.');
    if (auto o = GetMIMEDB_ ().cget ()->fSuffix2MediaTypeMap.Lookup (suffix)) {
        return *o;
    }
#if qPlatform_Windows
    using Characters::Format;
    using Configuration::Platform::Windows::RegistryKey;
    // only do registry lookup if needed, since (probably) more costly than local map lookup
    if (auto oct = RegistryKey{HKEY_CLASSES_ROOT}.Lookup (Format (L"%s\\Content Type", suffix.c_str ()))) {
        InternetMediaType mediaType{oct.As<String> ()};
        GetMIMEDB_ ().rwget ()->fSuffix2MediaTypeMap.Add (suffix, mediaType);
        return mediaType;
    }
#endif
    return nullopt;
}