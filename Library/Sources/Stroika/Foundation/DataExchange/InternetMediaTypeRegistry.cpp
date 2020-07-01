/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Cache/SynchronizedLRUCache.h"
#include "../Characters/Format.h"
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
#include "../IO/FileSystem/Common.h"
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

/*
 ********************************************************************************
 ******************** InternetMediaTypeRegistry::FrontendRep_ *******************
 ********************************************************************************
 */

/**
 *  @todo NYI UPDATING the frontend. Implement APIs to externally add mappings and be sure copying the InternetMediaTypeRegistry and using that
 *  in isolation works as well (use COW)
 */
//
struct InternetMediaTypeRegistry::FrontendRep_ : InternetMediaTypeRegistry::IFrontendRep_ {

    using IBackendRep = InternetMediaTypeRegistry::IBackendRep;

    // @todo REDO SO CACHING JUST IN FRONTEND, so no need in backend!!!! - remove the LRUCache from that layer....

    struct Data_ {
        shared_ptr<IBackendRep> fBackendRep;

        // NOTE - we cannot use Bijection, because multiple media-types can map to a single filetype and not all mediatypes have a filetype
        Mapping<FileSuffixType, InternetMediaType> fSuffix2MediaTypeMap;
        Mapping<InternetMediaType, FileSuffixType> fMediaType2PreferredSuffixMap;
        Mapping<InternetMediaType, String>         fMediaType2PrettyName;
    };
    mutable Synchronized<Data_> fData_; // lazy construct on first call to usage

    // NULL IS allowed - use that to on-dempand construct the backend
    FrontendRep_ (const shared_ptr<IBackendRep>& backendRep)
        : fData_{Data_{backendRep == nullptr ? nullptr : backendRep}}
    {
    }
    virtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const override
    {
        auto lockedData = fData_.rwget ();
        CheckData_ (&lockedData);
        if (auto o = lockedData->fMediaType2PreferredSuffixMap.Lookup (ct)) {
            return *o;
        }
        return lockedData->fBackendRep->GetPreferredAssociatedFileSuffix (ct);
    }
    virtual Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const override
    {
        auto lockedData = fData_.rwget ();
        CheckData_ (&lockedData);
        // @todo fetch all suffixes
        Set<String> result;
        if (auto i = GetPreferredAssociatedFileSuffix (ct)) {
            result += *i;
        }
        return result;
    }
    virtual optional<String> GetAssociatedPrettyName (const InternetMediaType& ct) const override
    {
        auto lockedData = fData_.rwget ();
        CheckData_ (&lockedData);
        return lockedData->fBackendRep->GetAssociatedPrettyName (ct);
    }
    virtual optional<InternetMediaType> GetAssociatedContentType (const FileSuffixType& fileSuffix) const override
    {
        Require (fileSuffix[0] == '.');
        auto lockedData = fData_.rwget ();
        CheckData_ (&lockedData);
        if (auto o = lockedData->fSuffix2MediaTypeMap.Lookup (fileSuffix)) {
            return *o;
        }
        return lockedData->fBackendRep->GetAssociatedContentType (fileSuffix);
    }
    static void CheckData_ (Synchronized<Data_>::WritableReference* lockedData)
    {
        if (lockedData->rwref ().fBackendRep == nullptr) {
            lockedData->rwref ().fBackendRep = InternetMediaTypeRegistry::DefaultBackend ();
        }
    }
};

/*
 ********************************************************************************
 *************************** InternetMediaTypeRegistry **************************
 ********************************************************************************
 */
InternetMediaTypeRegistry InternetMediaTypeRegistry::sThe;

InternetMediaTypeRegistry::InternetMediaTypeRegistry (const shared_ptr<IBackendRep>& backendRep)
    : fFrontEndRep_{make_shared<FrontendRep_> (backendRep)}
{
}

shared_ptr<InternetMediaTypeRegistry::IBackendRep> InternetMediaTypeRegistry::DefaultBackend ()
{
    Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::DefaultBackend"};
#if qPlatform_Windows
    return WindowsRegistryDefaultBackend ();
#endif
    // not sure how to tell if this works - @todo - FIX - need to avoid this on macos somehow...
    if (filesystem::exists ("/usr/share/mime")) {
        try {
            return UsrSharedDefaultBackend ();
        }
        catch (...) {
            // LOG/WRN
        }
    }
    // not sure how to tell if this works - @todo - FIX - need to avoid this on macos somehow...
    if (filesystem::exists ("/etc/mime.types")) {
        try {
            return EtcMimeTypesDefaultBackend ();
        }
        catch (...) {
            // LOG/WRN
        }
    }
    return BakedInDefaultBackend (); // always works (but sucks)
}

#if qPlatform_Windows
auto InternetMediaTypeRegistry::WindowsRegistryDefaultBackend () -> shared_ptr<IBackendRep>
{
    /*
     *  I can find no documentation on how this works, but at least https://stackoverflow.com/questions/3442607/mime-types-in-the-windows-registry
     *  mentions it.
     *
     *  Empirically you can find:
     *          HKEY_CLASSES_ROOT\MIME\Database
     *              Content Type\CT\Extension
     *              This layout does not appear to accomodate ever having more than one extension for a given mime type
     *
     *          HKEY_CLASSES_ROOT\FILE_SUFFIX
     *              {default} pretty name
     *              Content Type: 'internet media type'
     */
    Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::WindowsRegistryDefaultBackend"};
    struct WinRep_ : IBackendRep {
        // underlying windows code fast so use small cache sizes
        mutable Cache::SynchronizedLRUCache<FileSuffixType, optional<String>, equal_to<FileSuffixType>, hash<FileSuffixType>>                  fFileSuffix2PrettyNameCache_{25, 7};
        mutable Cache::SynchronizedLRUCache<FileSuffixType, optional<InternetMediaType>, equal_to<FileSuffixType>, hash<FileSuffixType>>       fSuffix2MediaTypeCache_{25, 7};
        mutable Cache::SynchronizedLRUCache<InternetMediaType, optional<FileSuffixType>, equal_to<InternetMediaType>, hash<InternetMediaType>> fContentType2FileSuffixCache_{25, 7};

        virtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const override
        {
            return fContentType2FileSuffixCache_.LookupValue (ct, [] (const InternetMediaType& ct) -> optional<FileSuffixType> {
                if (auto fs = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.Lookup (Characters::Format (L"MIME\\Database\\Content Type\\%s\\Extension", ct.As<String> ().c_str ()))) {
                    return fs.As<String> ();
                }
                return nullopt;
            });
        }
        virtual Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const override
        {
            // On Windows, for this registry format (see docs above class definition) - this only supports one suffix per content type
            Set<String> result;
            if (auto i = GetPreferredAssociatedFileSuffix (ct)) {
                result += *i;
            }
            return result;
        }
        virtual optional<String> GetAssociatedPrettyName (const InternetMediaType& ct) const override
        {
            if (optional<FileSuffixType> fileSuffix = GetPreferredAssociatedFileSuffix (ct)) {
                return fFileSuffix2PrettyNameCache_.LookupValue (*fileSuffix, [] (const String& suffix) -> optional<String> {
                    if (auto fileTypeID = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.Lookup (suffix + L"\\")) {
                        if (auto prettyName = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.Lookup (fileTypeID.As<String> () + L"\\")) {
                            return prettyName.As<String> ();
                        }
                    }
                    return nullopt;
                });
            }
            return nullopt;
        }
        virtual optional<InternetMediaType> GetAssociatedContentType (const FileSuffixType& fileSuffix) const override
        {
            Require (fileSuffix[0] == '.');
            return fSuffix2MediaTypeCache_.LookupValue (fileSuffix, [] (const FileSuffixType& fileSuffix) -> optional<InternetMediaType> {
                using Characters::Format;
                using Configuration::Platform::Windows::RegistryKey;
                // only do registry lookup if needed, since (probably) more costly than local map lookup
                if (auto oct = RegistryKey{HKEY_CLASSES_ROOT}.Lookup (Format (L"%s\\Content Type", fileSuffix.c_str ()))) {
                    InternetMediaType mediaType{oct.As<String> ()};
                    return mediaType;
                }
                return nullopt;
            });
        }
    };
    return make_shared<WinRep_> ();
}
#endif

auto InternetMediaTypeRegistry::EtcMimeTypesDefaultBackend () -> shared_ptr<IBackendRep>
{
    Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::EtcMimeTypesDefaultBackend"};
    /*
     *  Use the file /etc/mime.types
     *
     *  not sure this is useful - not sure who uses it that doesn't support /usr/share/mime...
     *
     *  And this is much less complete.
     *
     *  Preload the entire DB since its not practical to scan looking for the intended record (due to the time this would take).
     */
    struct EtcMimeTypesRep_ : IBackendRep {
        // NOTE - we cannot use Bijection, because multiple media-types can map to a single filetype and not all mediatypes have a filetype
        Mapping<FileSuffixType, InternetMediaType> fSuffix2MediaTypeMap_;
        Mapping<InternetMediaType, FileSuffixType> fMediaType2PreferredSuffixMap_;

        EtcMimeTypesRep_ ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"InternetMediaTypeRegistry::{}::EtcMimeTypesRep_::CTOR"};
#endif
            for (Sequence<String> line : DataExchange::Variant::CharacterDelimitedLines::Reader{{' ', '\t'}}.ReadMatrix (IO::FileSystem::FileInputStream::New (L"/etc/mime.types"))) {
                if (line.length () >= 2 and not line[0].StartsWith (L"#")) {
                    InternetMediaType ct{line[0]};
                    // a line starts with a content type, but then contains any number of file suffixes (without the leading .)
                    for (size_t i = 1; i < line.length (); ++i) {
                        String suffix = L"."sv + line[i];
                        fSuffix2MediaTypeMap_.Add (suffix, ct);
                        fMediaType2PreferredSuffixMap_.AddIf (ct, suffix, false);
                    }
                }
            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"succeeded with %d fSuffix2MediaTypeMap entries, and %d fMediaType2PreferredSuffixMap entries", fSuffix2MediaTypeMap_.size (), fMediaType2PreferredSuffixMap_.size ());
#endif
        }
        virtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const override
        {
            if (auto o = fMediaType2PreferredSuffixMap_.Lookup (ct)) {
                return *o;
            }
            return nullopt;
        }
        virtual Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const override
        {
            // @todo fetch all suffixes - but we don't currently store that.
            Set<String> result;
            if (auto i = fMediaType2PreferredSuffixMap_.Lookup (ct)) {
                result += *i;
            }
            return result;
        }
        virtual optional<String> GetAssociatedPrettyName (const InternetMediaType& /*ct*/) const override
        {
            return nullopt; // not supported in this file
        }
        virtual optional<InternetMediaType> GetAssociatedContentType (const FileSuffixType& fileSuffix) const override
        {
            Require (fileSuffix[0] == '.');
            if (auto o = fSuffix2MediaTypeMap_.Lookup (fileSuffix)) {
                return *o;
            }
            return nullopt;
        }
    };
    return make_shared<EtcMimeTypesRep_> ();
}

auto InternetMediaTypeRegistry::UsrSharedDefaultBackend () -> shared_ptr<IBackendRep>
{
    Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::UsrSharedDefaultBackend"};
    /*
     *  Documented to some small degree in https://www.linuxtopia.org/online_books/linux_desktop_guides/gnome_2.14_admin_guide/mimetypes-database.html
     *
     *  @todo Consider ALSO checking ~/... location and /usr/local ... location...
     */
    struct UsrShareMIMERep_ : IBackendRep {
        Iterable<filesystem::path> fDataRoots_{"~/.local/share/mime/"sv, "/usr/local/share/mime/"sv, "/usr/share/mime"sv};

        /*
         *  NOTE - for fSuffix2MediaTypeMap_ and fMediaType2PreferredSuffixMap, we cannot use Bijection, 
         *  because multiple media-types can map to a single filetype and not all mediatypes have a filetype.
         *
         *  We CANNOT use a cache, or dynamically fetch this data from files, because the data for each file suffix
         *  is not indexed (by file suffix) - its indexed by content type (so those lookups COULD be dynamic). But
         *  we can easily construct both at the same time reading the summary file, so we do.
         */
        Mapping<FileSuffixType, InternetMediaType> fSuffix2MediaTypeMap_;
        Mapping<InternetMediaType, FileSuffixType> fMediaType2PreferredSuffixMap_;

        mutable Synchronized<Mapping<InternetMediaType, String>> fMediaType2PrettyNameCache; // incrementally build as needed

        UsrShareMIMERep_ ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"InternetMediaTypeRegistry::{}UsrShareMIMERep_::CTOR"};
#endif
            // @todo consider using globs2 file support, but little point since they seem to be written in priority order
            auto loadGlobsFromFile = [&] (const filesystem::path& fn) {
                if (filesystem::exists (fn)) {
                    DbgTrace (L"%s exists so trying to load", Characters::ToString (fn).c_str ());
                    try {
                        for (Sequence<String> line : DataExchange::Variant::CharacterDelimitedLines::Reader{{':'}}.ReadMatrix (IO::FileSystem::FileInputStream::New (fn))) {
                            if (line.length () == 2) {
                                String glob = line[1];
                                if (glob.StartsWith ('*')) {
                                    glob = glob.SubString (1);
                                }
                                // Use AddIf () - so first (appears empirically to be the preferred value) wins
                                fSuffix2MediaTypeMap_.AddIf (glob, InternetMediaType{line[0]}, false);
                                fMediaType2PreferredSuffixMap_.AddIf (InternetMediaType{line[0]}, glob, false);

                                // @todo add support to track all associated suffixes for mime type
                            }
                        }
                    }
                    catch (...) {
                        // log error
                    }
                }
            };
            // override files loaded first, tied to use of AddIf - not replacing
            for (auto p : fDataRoots_) {
                loadGlobsFromFile (p / "globs");
            }

#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"succeeded with %d fSuffix2MediaTypeMap_ entries, and %d fMediaType2PreferredSuffixMap entries", fSuffix2MediaTypeMap_.size (), fMediaType2PreferredSuffixMap_.size ());
#endif
        }
        virtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const override
        {
            if (auto o = fMediaType2PreferredSuffixMap_.Lookup (ct)) {
                return *o;
            }
            return nullopt;
        }
        virtual Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const override
        {
            // @todo fetch all suffixes
            Set<String> result;
            if (auto i = fMediaType2PreferredSuffixMap_.Lookup (ct)) {
                result += *i;
            }
            return result;
        }
        virtual optional<String> GetAssociatedPrettyName (const InternetMediaType& ct) const override
        {
            return LookupAndUpdateFromUsrShareMimePrettyName_ (ct);
        }
        virtual optional<InternetMediaType> GetAssociatedContentType (const FileSuffixType& fileSuffix) const override
        {
            Require (fileSuffix[0] == '.');
            if (auto o = fSuffix2MediaTypeMap_.Lookup (fileSuffix)) {
                return *o;
            }
            return nullopt;
        }
        optional<String> LookupAndUpdateFromUsrShareMimePrettyName_ (const InternetMediaType& ct) const
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"{}MIMEDB_::LookupAndUpdateFromUsrShareMimePrettyName"};
#endif
            // @todo combine lock calls in this procedure
            if (auto o = fMediaType2PrettyNameCache.cget ()->Lookup (ct)) {
                return *o;
            }
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
                filesystem::path mimeRoot{"/usr/share/mime/"};
                myHander_        handler;
                // @todo validate ct.GetType () to make sure not a ../../ ATTACK
                DataExchange::XML::SAXParse (IO::FileSystem::FileInputStream::New (mimeRoot / IO::FileSystem::ToPath (ct.GetType () + L"/" + ct.GetSubType () + L".xml")), handler);
                if (handler.fResult) {
                    fMediaType2PrettyNameCache.rwget ()->Add (ct, *handler.fResult);
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
    return make_shared<UsrShareMIMERep_> ();
}

auto InternetMediaTypeRegistry::BakedInDefaultBackend () -> shared_ptr<IBackendRep>
{
    Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::BakedInDefaultBackend"};
    struct BakedInTypesRep_ : IBackendRep {
        // NOTE - we cannot use Bijection, because multiple media-types can map to a single filetype and not all mediatypes have a filetype
        Mapping<FileSuffixType, InternetMediaType> fSuffix2MediaTypeMap_;
        Mapping<InternetMediaType, FileSuffixType> fMediaType2PreferredSuffixMap_;

        BakedInTypesRep_ ()
        {
            for (auto i : initializer_list<pair<InternetMediaType, FileSuffixType>>{
                     {InternetMediaTypes::kText_PLAIN, L".txt"},
                     {InternetMediaTypes::kText_HTML, L".htm"},
                     {InternetMediaTypes::kText_HTML, L".html"},
                     {InternetMediaTypes::kJSON, L".json"},
                 }) {
                fSuffix2MediaTypeMap_.AddIf (i.second, i.first);
                fMediaType2PreferredSuffixMap_.AddIf (i.first, i.second);
            }
        }
        virtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const override
        {
            if (auto o = fMediaType2PreferredSuffixMap_.Lookup (ct)) {
                return *o;
            }
            return nullopt;
        }
        virtual Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const override
        {
            Set<String> result;
            if (auto i = fMediaType2PreferredSuffixMap_.Lookup (ct)) {
                result += *i;
            }
            return result;
        }
        virtual optional<String> GetAssociatedPrettyName (const InternetMediaType& /*ct*/) const override
        {
            return nullopt; // not supported in this file
        }
        virtual optional<InternetMediaType> GetAssociatedContentType (const FileSuffixType& fileSuffix) const override
        {
            Require (fileSuffix[0] == '.');
            if (auto o = fSuffix2MediaTypeMap_.Lookup (fileSuffix)) {
                return *o;
            }
            return nullopt;
        }
    };
    return make_shared<BakedInTypesRep_> ();
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

Set<String> InternetMediaTypeRegistry::GetAssociatedFileSuffixes (const Iterable<InternetMediaType>& cts) const
{
    Set<InternetMediaType> mediaTypes;
    cts.Apply ([&mediaTypes, this] (auto i) {
        mediaTypes += GetMoreGeneralTypes (i);
    });
    Set<String> result;
    for (auto ct : mediaTypes) {
        for (auto i : GetAssociatedFileSuffixes (ct)) {
            result += i;
        }
    }
    return result;
}

optional<InternetMediaType> InternetMediaTypeRegistry::GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const
{
    FileSuffixType suffix = IO::FileSystem::FromPath (IO::FileSystem::ToPath (fileNameOrSuffix).extension ());
    if (suffix.empty ()) {
        return nullopt;
    }
    Assert (suffix[0] == '.');
    return fFrontEndRep_->GetAssociatedContentType (suffix);
}