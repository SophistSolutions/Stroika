/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>

#include "Stroika/Foundation/Cache/SynchronizedLRUCache.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "Stroika/Foundation/DataExchange/XML/SAXReader.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Configuration/Platform/Windows/Registry.h"
#endif
#include "Stroika/Foundation/Debug/Trace.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"

#include "InternetMediaTypeRegistry.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;

using Memory::NullCoalesce;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using FileSuffixType = InternetMediaTypeRegistry::FileSuffixType;

/*
 ********************************************************************************
 ****************** InternetMediaTypeRegistry::OverrideRecord *******************
 ********************************************************************************
 */
String InternetMediaTypeRegistry::OverrideRecord::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fTypePrintName) {
        sb << "TypePrintName: " << fTypePrintName << ", "sv;
    }
    if (fFileSuffixes) {
        sb << "FileSuffixes: " << fFileSuffixes << ", "sv;
    }
    if (fPreferredSuffix) {
        sb << "PreferredSuffix: " << fPreferredSuffix;
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ******************** InternetMediaTypeRegistry::FrontendRep_ *******************
 ********************************************************************************
 */

/**
 *  @todo NYI UPDATING the frontend. Implement APIs to externally add mappings and be sure copying the InternetMediaTypeRegistry and using that
 *  in isolation works as well (use COW)
 *
 *  \note - Structurally, we do caching in the backend (as needed) because each backend stores data differently
 *          and the logic of what makes sense to cache changes.
 *
 *          The frontend simply defines 'API-Driven OVERRIDES' of the values returned. (so far not fully implemented - no setters/manipulators)
 */
struct InternetMediaTypeRegistry::FrontendRep_ : InternetMediaTypeRegistry::IFrontendRep_ {

    using IBackendRep = InternetMediaTypeRegistry::IBackendRep;

    // Baked in predefined initial user-overrides.
    // These are adjustable by API, serve the purpose of providing a default on systems with no MIME content database -- LGP 2020-07-27

    static inline const Mapping<InternetMediaType, OverrideRecord> kDefaults_{initializer_list<KeyValuePair<InternetMediaType, OverrideRecord>>{
        {InternetMediaTypes::kText_PLAIN, OverrideRecord{nullopt, Containers::Set<String>{".txt"sv}, ".txt"sv}},
        {InternetMediaTypes::kCSS, OverrideRecord{nullopt, Containers::Set<String>{".css"sv}, ".css"sv}},
        {InternetMediaTypes::kHTML, OverrideRecord{nullopt, Containers::Set<String>{".htm"sv, ".html"sv}, ".htm"sv}},
        {InternetMediaTypes::kJavascript, OverrideRecord{nullopt, Containers::Set<String>{".js"sv}, ".js"sv}},
        {InternetMediaTypes::kJSON, OverrideRecord{nullopt, Containers::Set<String>{".json"sv}, ".json"sv}},
        {InternetMediaTypes::kPNG, OverrideRecord{nullopt, Containers::Set<String>{".png"sv}, ".png"sv}},
        {InternetMediaTypes::kXML, OverrideRecord{nullopt, Containers::Set<String>{".xml"sv}, ".xml"sv}},
    }};

    // OVERRIDE values (take precedence over backend) and any other data we need to keep locked (synchronized)
    struct Data_ {
        shared_ptr<IBackendRep> fBackendRep; // lazy construct on first call to usage (since that construction can be slow)

        Mapping<FileSuffixType, InternetMediaType> fSuffix2MediaTypeMap;
        Mapping<InternetMediaType, OverrideRecord> fOverrides;
    };
    mutable Synchronized<Data_> fData_;

    // NULL backendRep IS allowed - use that to on-demand construct the backend
    FrontendRep_ (const shared_ptr<IBackendRep>& backendRep)
        : FrontendRep_{backendRep, kDefaults_}
    {
    }
    FrontendRep_ (const shared_ptr<IBackendRep>& backendRep, const Mapping<InternetMediaType, OverrideRecord>& overrides)
        : fData_{Data_{.fBackendRep = backendRep}}
    {
        SetOverrides (overrides);
    }
    virtual Mapping<InternetMediaType, OverrideRecord> GetOverrides () const override
    {
        auto lockedData = fData_.rwget ();
        return lockedData->fOverrides;
    }
    virtual void SetOverrides (const Mapping<InternetMediaType, OverrideRecord>& overrides) override
    {
        auto lockedData        = fData_.rwget ();
        lockedData->fOverrides = overrides;
        lockedData->fSuffix2MediaTypeMap.clear ();
        for (const auto& i : lockedData->fOverrides) {
            if (i.fValue.fFileSuffixes) {
                for (const auto& si : *i.fValue.fFileSuffixes) {
                    lockedData->fSuffix2MediaTypeMap.Add (si, i.fKey, AddReplaceMode::eAddIfMissing);
                }
            }
        }
    }
    virtual void AddOverride (const InternetMediaType& mediaType, const OverrideRecord& overrideRec) override
    {
        auto lockedData = fData_.rwget ();
        lockedData->fOverrides.Add (mediaType, overrideRec);
        lockedData->fSuffix2MediaTypeMap.clear ();
        for (const auto& i : lockedData->fOverrides) {
            if (i.fValue.fFileSuffixes) {
                for (const auto& si : *i.fValue.fFileSuffixes) {
                    lockedData->fSuffix2MediaTypeMap.Add (si, i.fKey, AddReplaceMode::eAddIfMissing);
                }
            }
        }
    }
    virtual shared_ptr<IBackendRep> GetBackendRep () const override
    {
        auto lockedData = fData_.rwget ();
        return lockedData->fBackendRep;
    }
    virtual Containers::Set<InternetMediaType> GetMediaTypes (optional<InternetMediaType::AtomType> majorType) const override
    {
        using AtomType  = InternetMediaType::AtomType;
        auto lockedData = fData_.rwget ();
        CheckData_ (&lockedData);
        Containers::Set<InternetMediaType> result = lockedData->fBackendRep->GetMediaTypes (majorType);
        if (majorType == nullopt) {
            result += lockedData->fOverrides.Keys ();
        }
        else {
            lockedData->fOverrides.Keys ().Apply ([&] (const InternetMediaType& i) {
                if (i.GetType<AtomType> () == majorType) {
                    result += i;
                }
            });
        }
        return result;
    }
    virtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const override
    {
        auto lockedData = fData_.rwget ();
        CheckData_ (&lockedData);
        if (auto o = lockedData->fOverrides.Lookup (ct)) {
            if (o->fPreferredSuffix) {
                return *o->fPreferredSuffix;
            }
        }
        return lockedData->fBackendRep->GetPreferredAssociatedFileSuffix (ct);
    }
    virtual Containers::Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const override
    {
        auto lockedData = fData_.rwget ();
        CheckData_ (&lockedData);
        Containers::Set<String> result = lockedData->fOverrides.LookupValue (ct).fFileSuffixes.value_or (Containers::Set<FileSuffixType>{});
        result += lockedData->fBackendRep->GetAssociatedFileSuffixes (ct);
        return result;
    }
    virtual optional<String> GetAssociatedPrettyName (const InternetMediaType& ct) const override
    {
        auto lockedData = fData_.rwget ();
        CheckData_ (&lockedData);
        if (auto o = lockedData->fOverrides.Lookup (ct)) {
            if (o->fTypePrintName) {
                return *o->fTypePrintName;
            }
        }
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
inline InternetMediaTypeRegistry::FrontendRep_ InternetMediaTypeRegistry::kDefaultFrontEndForNoBackend_{nullptr};

/*
 ********************************************************************************
 ******************** InternetMediaTypeRegistry::Rep_Cloner_ ********************
 ********************************************************************************
 */
auto InternetMediaTypeRegistry::Rep_Cloner_::operator() (const IFrontendRep_& t) const -> shared_ptr<IFrontendRep_>
{
    return make_shared<FrontendRep_> (t.GetBackendRep (), t.GetOverrides ());
};

/*
 ********************************************************************************
 *************************** InternetMediaTypeRegistry **************************
 ********************************************************************************
 */
InternetMediaTypeRegistry::InternetMediaTypeRegistry (const shared_ptr<IBackendRep>& backendRep)
    // note because can be constructed before main () - not safe to make_shared<FrontendRep_> - so delay construction and use kDefaultFrontEndForNoBackend_ if needed
    : fFrontEndRep_{backendRep == nullptr ? nullptr : make_shared<FrontendRep_> (backendRep)}
{
}

auto InternetMediaTypeRegistry::GetOverrides () const -> Mapping<InternetMediaType, OverrideRecord>
{
    return NullCoalesce (fFrontEndRep_, kDefaultFrontEndForNoBackend_).GetOverrides ();
}

void InternetMediaTypeRegistry::SetOverrides (const Mapping<InternetMediaType, OverrideRecord>& overrides)
{
    if (fFrontEndRep_ == nullptr) {
        fFrontEndRep_ = make_shared<FrontendRep_> (kDefaultFrontEndForNoBackend_);
    }
    AssertNotNull (fFrontEndRep_);
    fFrontEndRep_->SetOverrides (overrides);
}

void InternetMediaTypeRegistry::AddOverride (const InternetMediaType& mediaType, const OverrideRecord& overrideRec)
{
    if (fFrontEndRep_ == nullptr) {
        fFrontEndRep_ = make_shared<FrontendRep_> (kDefaultFrontEndForNoBackend_);
    }
    AssertNotNull (fFrontEndRep_);
    fFrontEndRep_->AddOverride (mediaType, overrideRec);
}
optional<InternetMediaTypeRegistry::FileSuffixType> InternetMediaTypeRegistry::GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const
{
    return Memory::NullCoalesce (fFrontEndRep_, kDefaultFrontEndForNoBackend_).GetPreferredAssociatedFileSuffix (ct);
}
Containers::Set<String> InternetMediaTypeRegistry::GetAssociatedFileSuffixes (const InternetMediaType& ct) const
{
    return Memory::NullCoalesce (fFrontEndRep_, kDefaultFrontEndForNoBackend_).GetAssociatedFileSuffixes (ct);
}
optional<String> InternetMediaTypeRegistry::GetAssociatedPrettyName (const InternetMediaType& ct) const
{
    return Memory::NullCoalesce (fFrontEndRep_, kDefaultFrontEndForNoBackend_).GetAssociatedPrettyName (ct);
}

shared_ptr<InternetMediaTypeRegistry::IBackendRep> InternetMediaTypeRegistry::DefaultBackend ()
{
    Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::DefaultBackend"};
#if qPlatform_Windows
    return WindowsRegistryDefaultBackend ();
#endif
    // @todo fix for MacOS - which doesn't support these - http://stroika-bugs.sophists.com/browse/STK-795
    if (filesystem::exists ("/usr/share/mime"sv)) {
        try {
            return UsrSharedDefaultBackend ();
        }
        catch (...) {
            // LOG/WRN
        }
    }
    if (filesystem::exists ("/etc/mime.types"sv)) {
        try {
            return EtcMimeTypesDefaultBackend ();
        }
        catch (...) {
            // LOG/WRN
        }
    }
    return BakedInDefaultBackend (); // always works (but sucks)
}

auto InternetMediaTypeRegistry::EtcMimeTypesDefaultBackend () -> shared_ptr<IBackendRep>
{
    Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::EtcMimeTypesDefaultBackend"};
    /*
     *  Use the file /etc/mime.types
     *
     *  not sure this is useful - not sure who uses it that doesn't support /usr/share/mime...
     *
     *  Preload the entire DB since its not practical to scan looking for the intended record (due to the time this would take).
     */
    struct EtcMimeTypesRep_ : IBackendRep {
        Mapping<FileSuffixType, InternetMediaType>                  fSuffix2MediaTypeMap_;
        Mapping<InternetMediaType, FileSuffixType>                  fMediaType2PreferredSuffixMap_;
        Mapping<InternetMediaType, Containers::Set<FileSuffixType>> fMediaType2SuffixesMap_;

        EtcMimeTypesRep_ ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::{}::EtcMimeTypesRep_::CTOR"};
#endif
            for (Sequence<String> line : DataExchange::Variant::CharacterDelimitedLines::Reader{{' ', '\t'}}.ReadMatrix (
                     IO::FileSystem::FileInputStream::New ("/etc/mime.types"sv))) {
                if (line.length () >= 2 and not line[0].StartsWith ("#"_k)) {
                    InternetMediaType ct;
                    try {
                        ct = InternetMediaType{line[0]};
                    }
                    catch (...) {
                        DbgTrace ("Ignoring exception looking parsing potential media type entry ({}): {}"_f, line[0], current_exception ());
                    }
                    // a line starts with a content type, but then contains any number of file suffixes (without the leading .)
                    Containers::Set<FileSuffixType> fileSuffixes;
                    for (size_t i = 1; i < line.length (); ++i) {
                        Assert (not line[i].empty ());
                        String suffix = "."sv + line[i];
                        fSuffix2MediaTypeMap_.Add (suffix, ct);
                        fMediaType2PreferredSuffixMap_.Add (ct, suffix, AddReplaceMode::eAddIfMissing);
                        fileSuffixes.Add (suffix);
                    }
                    fMediaType2SuffixesMap_.Add (ct, fileSuffixes);
                }
            }
            // Because on raspberrypi/debian, this comes out with a crazy default for text\plain -- LGP 2020-07-27
            fMediaType2PreferredSuffixMap_.Add (InternetMediaTypes::kText_PLAIN, ".txt"sv);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"succeeded with {} fSuffix2MediaTypeMap entries, and {} fMediaType2PreferredSuffixMap entries"_f,
                      fSuffix2MediaTypeMap_.size (), fMediaType2PreferredSuffixMap_.size ());
#endif
        }
        virtual Containers::Set<InternetMediaType> GetMediaTypes (optional<InternetMediaType::AtomType> majorType) const override
        {
            Containers::Set<InternetMediaType> results;
            for (const InternetMediaType& imt : fMediaType2PreferredSuffixMap_.Keys ()) {
                if (majorType != nullopt and (imt.GetType<InternetMediaType::AtomType> () != *majorType)) {
                    continue;
                }
                results += imt;
            }
            return results;
        }
        virtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const override
        {
            if (auto o = fMediaType2PreferredSuffixMap_.Lookup (ct)) {
                return *o;
            }
            return nullopt;
        }
        virtual Containers::Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const override
        {
            if (auto i = fMediaType2SuffixesMap_.Lookup (ct)) {
                return *i;
            }
            return Containers::Set<String>{};
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
     */
    struct UsrShareMIMERep_ : IBackendRep {
        Iterable<filesystem::path> fDataRoots_{"~/.local/share/mime/"sv, "/usr/local/share/mime/"sv, "/usr/share/mime"sv};

        /*
         *  NOTE - for fSuffix2MediaTypeMap_ and fMediaType2PreferredSuffixMap, we cannot use Bijection, 
         *  because multiple media-types can map to a single filetype and not all mediatypes have a filetype.
         *
         *  We CANNOT use a cache, or dynamically fetch this data from files, because the data for each file suffix
         *  is not indexed (by file suffix) - it is indexed by content type (so those lookups COULD be dynamic). But
         *  we can easily construct both at the same time reading the summary file, so we do.
         */
        Mapping<FileSuffixType, InternetMediaType>                  fSuffix2MediaTypeMap_;
        Mapping<InternetMediaType, FileSuffixType>                  fMediaType2PreferredSuffixMap_;
        Mapping<InternetMediaType, Containers::Set<FileSuffixType>> fMediaType2SuffixesMap_;

        mutable Synchronized<Mapping<InternetMediaType, String>> fMediaType2PrettyNameCache; // incrementally build as needed

        UsrShareMIMERep_ ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::{}UsrShareMIMERep_::CTOR"};
#endif
            // @todo consider using globs2 file support, but little point since they seem to be written in priority order
            auto loadGlobsFromFile = [&] (const filesystem::path& fn) {
                if (filesystem::exists (fn)) {
                    Debug::TraceContextBumper ctx1{"UsrShareMIMERep_::CTOR::loadGlobsFromFile", "exists=true,fn={}"_f, fn};
                    try {
                        for (Sequence<String> line :
                             DataExchange::Variant::CharacterDelimitedLines::Reader{{':'}}.ReadMatrix (IO::FileSystem::FileInputStream::New (fn))) {
                            if (line.length () == 2) {
                                String glob = line[1];
                                if (glob.StartsWith ('*')) {
                                    glob = glob.SubString (1);
                                }
                                // Use AddReplaceMode::eAddIfMissing - so first (appears empirically to be the preferred value) wins
                                InternetMediaType imt;
                                try {
                                    imt = InternetMediaType{line[0]};
                                }
                                catch (...) {
                                    DbgTrace ("Ignoring exception looking parsing potential media type entry ({}): {}"_f, line[0], current_exception ());
                                }
                                fSuffix2MediaTypeMap_.Add (glob, imt, AddReplaceMode::eAddIfMissing);
                                fMediaType2PreferredSuffixMap_.Add (imt, glob, AddReplaceMode::eAddIfMissing);

                                // update the set of mapped suffixes
                                Containers::Set<FileSuffixType> prevSuffixes = fMediaType2SuffixesMap_.LookupValue (imt);
                                prevSuffixes.Add (glob);
                                fMediaType2SuffixesMap_.Add (imt, prevSuffixes);
                            }
                        }

                        // Because on raspberrypi/debian, this comes out with a crazy default for text\plain -- LGP 2020-07-27
                        fMediaType2PreferredSuffixMap_.Add (InternetMediaTypes::kText_PLAIN, ".txt"_k);
                    }
                    catch (...) {
                        // log error
                    }
                }
            };
            // override files loaded first, tied to use of AddReplaceMode::eAddIfMissing - not replacing
            for (const auto& p : fDataRoots_) {
                loadGlobsFromFile (p / "globs");
            }

#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("succeeded with {} fSuffix2MediaTypeMap_ entries, and {} fMediaType2PreferredSuffixMap entries"_f,
                      fSuffix2MediaTypeMap_.size (), fMediaType2PreferredSuffixMap_.size ());
#endif
        }
        virtual Containers::Set<InternetMediaType> GetMediaTypes (optional<InternetMediaType::AtomType> majorType) const override
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("UsrShareMIMERep_::GetMediaTypes", "majorType={}"_f, majorType)};
#endif
            Containers::Set<InternetMediaType> results;
            for (const auto& imt : fMediaType2PreferredSuffixMap_.Keys ()) {
                if (majorType) {
                    if (imt.GetType<InternetMediaType::AtomType> () != *majorType) {
                        continue; // skip non-matching types
                    }
                }
                results += imt;
            }
            return results;
        }
        virtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const override
        {
            if (auto o = fMediaType2PreferredSuffixMap_.Lookup (ct)) {
                return *o;
            }
            return nullopt;
        }
        virtual Containers::Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const override
        {
            if (auto i = fMediaType2SuffixesMap_.Lookup (ct)) {
                return *i;
            }
            return Containers::Set<FileSuffixType>{};
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
            Debug::TraceContextBumper ctx{"{}MIMEDB_::LookupAndUpdateFromUsrShareMimePrettyName"};
#endif
            // @todo combine lock calls in this procedure
            if (auto o = fMediaType2PrettyNameCache.cget ()->Lookup (ct)) {
                return *o;
            }
            // SAX parse /usr/share/mime/TYPE/SUBTYPE.xml file and look for <comment> element (default with no language for now)
            // Simpler - just take the first - seems empirically fine/OK
#if qStroika_Foundation_DataExchange_XML_SupportParsing
            try {
                using Name = StructuredStreamEvents::Name;
                struct myHander_ : StructuredStreamEvents::IConsumer {
                    optional<String> fResult;
                    bool             onContentElt{false};
                    StringBuilder<>  fAccum;
                    virtual void     StartElement (const Name& name, [[maybe_unused]] const Mapping<Name, String>& attributes) override
                    {
                        if (name == Name{"content"_k} and not fResult.has_value ()) {
                            onContentElt = true;
                        }
                    }
                    virtual void EndElement ([[maybe_unused]] const Name& name) override
                    {
                        if (onContentElt) {
                            Assert (not fResult);
                            fResult = fAccum.str ();
                        }
                    }
                    virtual void TextInsideElement (const String& t) override
                    {
                        if (onContentElt) {
                            fAccum << t;
                        }
                    }
                };
                filesystem::path mimeRoot{"/usr/share/mime/"sv};
                myHander_        handler;
                // @todo validate ct.GetType () to make sure not a ../../ ATTACK
                DataExchange::XML::SAXParse (IO::FileSystem::FileInputStream::New (
                                                 mimeRoot / IO::FileSystem::ToPath (ct.GetType () + "/"_k + ct.GetSubType () + ".xml"_k)),
                                             &handler);
                if (handler.fResult) {
                    fMediaType2PrettyNameCache.rwget ()->Add (ct, *handler.fResult);
                    return *handler.fResult;
                }
            }
            catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("failure ignored");
#endif
            }
#else
            DbgTrace ("/usr/share/mime/ ignored cuz no xml reader - not compiled with Xerces");
#endif
            return nullopt;
        }
    };
    return make_shared<UsrShareMIMERep_> ();
}

auto InternetMediaTypeRegistry::BakedInDefaultBackend () -> shared_ptr<IBackendRep>
{
    Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::BakedInDefaultBackend"};
    struct DefaultEmptyBackendRep_ : IBackendRep {
        virtual Containers::Set<InternetMediaType> GetMediaTypes ([[maybe_unused]] optional<InternetMediaType::AtomType> majorType) const override
        {
            return Containers::Set<InternetMediaType>{};
        }
        virtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix ([[maybe_unused]] const InternetMediaType& ct) const override
        {
            return nullopt;
        }
        virtual Containers::Set<FileSuffixType> GetAssociatedFileSuffixes ([[maybe_unused]] const InternetMediaType& ct) const override
        {
            return Containers::Set<String>{};
        }
        virtual optional<String> GetAssociatedPrettyName (const InternetMediaType& /*ct*/) const override
        {
            return nullopt;
        }
        virtual optional<InternetMediaType> GetAssociatedContentType ([[maybe_unused]] const FileSuffixType& fileSuffix) const override
        {
            Require (fileSuffix[0] == '.');
            return nullopt;
        }
    };
    return make_shared<DefaultEmptyBackendRep_> ();
}

#if qPlatform_Windows
auto InternetMediaTypeRegistry::WindowsRegistryDefaultBackend () -> shared_ptr<IBackendRep>
{
    /*
     *  I can find no documentation on how this works, but at least https://stackoverflow.com/questions/3442607/mime-types-in-the-windows-registry
     *  mentions it.
     *
     *  Empirically you can usually find:
     *          HKEY_CLASSES_ROOT\MIME\Database
     *              Content Type\CT\Extension
     *              This layout does not appear to accommodate ever having more than one extension for a given mime type
     *
     *          HKEY_CLASSES_ROOT\FILE_SUFFIX
     *              {default} pretty name
     *              Content Type: 'internet media type'
     *
     *  \note On Docker windows server core images, this is often missing! (but addressed with the default values baked into the frontend) -- LGP 2020-07-28
     */
    Debug::TraceContextBumper ctx{"InternetMediaTypeRegistry::WindowsRegistryDefaultBackend"};
    struct WinRep_ : IBackendRep {
        // underlying windows code fast so use small cache sizes
        mutable Cache::SynchronizedLRUCache<FileSuffixType, optional<String>, equal_to<FileSuffixType>, hash<FileSuffixType>> fFileSuffix2PrettyNameCache_{
            25, 7};
        mutable Cache::SynchronizedLRUCache<FileSuffixType, optional<InternetMediaType>, equal_to<FileSuffixType>, hash<FileSuffixType>> fSuffix2MediaTypeCache_{
            25, 7};
        mutable Cache::SynchronizedLRUCache<InternetMediaType, optional<FileSuffixType>, equal_to<InternetMediaType>, hash<InternetMediaType>> fContentType2FileSuffixCache_{
            25, 7};
        mutable Cache::SynchronizedLRUCache<InternetMediaType, Containers::Set<FileSuffixType>, equal_to<InternetMediaType>, hash<InternetMediaType>> fContentType2FileSuffixesCache_{
            25, 7};

        virtual Containers::Set<InternetMediaType> GetMediaTypes (optional<InternetMediaType::AtomType> majorType) const override
        {
            Containers::Set<InternetMediaType> result;
            //
            // rarely do we fetch all MIME types, so don't cache - just re-fetch each time
            //
            // On Windows, in registry, easiest way appears to be to enumerate ALL registry entries in HKCR that start with .,
            // and look for sub-field 'Content-type'
            //
            using RegistryKey = Configuration::Platform::Windows::RegistryKey;
            for (shared_ptr<RegistryKey> sk : RegistryKey{HKEY_CLASSES_ROOT}.EnumerateSubKeys ()) {
                String name = sk->GetFullPathOfKey ().Tokenize ({'\\'}).LastValue ();
                if (name.StartsWith ('.')) {
                    if (auto o = sk->Lookup ("Content Type"sv)) {
                        InternetMediaType imt;
                        try {
                            imt = InternetMediaType{o.As<String> ()};
                        }
                        catch (...) {
                            // ignore bad format - such as .sqlproj has Content-Type "string" which my read of the RFC says is illegal
                            DbgTrace ("Ignoring exception parsing registry key ({}): {}"_f, o, current_exception ());
                            continue;
                        }
                        if (majorType) {
                            if (imt.GetType<InternetMediaType::AtomType> () != *majorType) {
                                continue; // skip non-matching types
                            }
                        }
                        result.Add (imt);
                    }
                }
            }
            return result;
        }
        virtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const override
        {
            return fContentType2FileSuffixCache_.LookupValue (ct, [] (const InternetMediaType& ct) -> optional<FileSuffixType> {
                if (auto fs = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.Lookup (
                        Characters::Format ("MIME\\Database\\Content Type\\{}\\Extension"_f, ct))) {
                    return fs.As<String> ();
                }
                return nullopt;
            });
        }
        virtual Containers::Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const override
        {
            // This is expensive to compute, and we could compute all and cache, but I don't think we will need to lookup very often, so just
            // compute as needed and cache a few
            return fContentType2FileSuffixesCache_.LookupValue (ct, [] (const InternetMediaType& ct) -> Containers::Set<FileSuffixType> {
                Containers::Set<FileSuffixType> result;
                using Configuration::Platform::Windows::RegistryKey;
                for (shared_ptr<RegistryKey> sk : RegistryKey{HKEY_CLASSES_ROOT}.EnumerateSubKeys ()) {
                    String name = sk->GetFullPathOfKey ().Tokenize ({'\\'}).LastValue ();
                    if (name.StartsWith ("."_k)) {
                        if (auto o = sk->Lookup ("Content Type"sv)) {
                            InternetMediaType imt;
                            try {
                                imt = InternetMediaType{o.As<String> ()};
                            }
                            catch (...) {
                                // ignore bad format - such as .sqlproj has Content-Type "string" which my read of the RFC says is illegal
                                DbgTrace ("Ignoring exception parsing registry key ({}): {}"_f, o, current_exception ());
                                continue;
                            }
                            if (ct.GetType () == imt.GetType () and ct.GetSubType () == imt.GetSubType ()) {
                                result += name;
                            }
                        }
                    }
                }
                return result;
            });
        }
        virtual optional<String> GetAssociatedPrettyName (const InternetMediaType& ct) const override
        {
            if (optional<FileSuffixType> fileSuffix = GetPreferredAssociatedFileSuffix (ct)) {
                return fFileSuffix2PrettyNameCache_.LookupValue (*fileSuffix, [] (const String& suffix) -> optional<String> {
                    if (auto fileTypeID = Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.Lookup (suffix + "\\"_k)) {
                        if (auto prettyName =
                                Configuration::Platform::Windows::RegistryKey{HKEY_CLASSES_ROOT}.Lookup (fileTypeID.As<String> () + "\\"_k)) {
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
                if (auto oct = RegistryKey{HKEY_CLASSES_ROOT}.Lookup (Format ("{}\\Content Type"_f, fileSuffix))) {
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

Set<InternetMediaType> InternetMediaTypeRegistry::GetMediaTypes () const
{
    return NullCoalesce (fFrontEndRep_, kDefaultFrontEndForNoBackend_).GetMediaTypes (nullopt);
}

Set<InternetMediaType> InternetMediaTypeRegistry::GetMediaTypes (InternetMediaType::AtomType majorType) const
{
    return NullCoalesce (fFrontEndRep_, kDefaultFrontEndForNoBackend_).GetMediaTypes (majorType);
}

Set<String> InternetMediaTypeRegistry::GetAssociatedFileSuffixes (const Iterable<InternetMediaType>& mediaTypes) const
{
    Containers::Set<String> result;
    for (const auto& ct : mediaTypes) {
        for (const auto& i : GetAssociatedFileSuffixes (ct)) {
            result += i;
        }
    }
    return result;
}

optional<InternetMediaType> InternetMediaTypeRegistry::GetAssociatedContentType (const FileSuffixType& fileSuffix) const
{
    if (fileSuffix.empty ()) {
        return nullopt;
    }
    Assert (fileSuffix[0] == '.');
    return NullCoalesce (fFrontEndRep_, kDefaultFrontEndForNoBackend_).GetAssociatedContentType (fileSuffix);
}

bool InternetMediaTypeRegistry::IsTextFormat (const InternetMediaType& ct) const
{
    // @todo look into info in type files/backend to make extensible
    using AtomType = InternetMediaType::AtomType;
    if (ct.GetType<AtomType> () == InternetMediaTypes::Types::kText) {
        return true;
    }
    if (IsXMLFormat (ct)) {
        return true;
    }
    // well known types that can be treated as text (@todo need some way to extend this API)? - Maybe not here but in REGISTRY
    if (ct.GetType<AtomType> () == InternetMediaTypes::Types::kApplication) {
        Assert (InternetMediaTypes::kJSON.GetType<AtomType> () == InternetMediaTypes::Types::kApplication);
        if (ct.GetSubType<AtomType> () == InternetMediaTypes::kJSON.GetSubType<AtomType> ()) {
            return true;
        }
        Assert (InternetMediaTypes::kXSLT.GetType<AtomType> () == InternetMediaTypes::Types::kApplication);
        if (ct.GetSubType<AtomType> () == InternetMediaTypes::kXSLT.GetSubType<AtomType> ()) {
            return true;
        }
        Assert (InternetMediaTypes::kRTF.GetType<AtomType> () == InternetMediaTypes::Types::kApplication);
        if (ct.GetSubType<AtomType> () == InternetMediaTypes::kRTF.GetSubType<AtomType> ()) {
            return true;
        }
    }
    return false;
}

bool InternetMediaTypeRegistry::IsImageFormat (const InternetMediaType& ct) const
{
    // @todo look into info in type files/backend to make extensible
    using AtomType = InternetMediaType::AtomType;
    if (ct.GetType<AtomType> () == InternetMediaTypes::Types::kImage) {
        return true;
    }
    /*
     * TODO:
     *      o   NEED EXTENSION MECHANSIM TO ADD OTHER TYPES
     */
    return false;
}

bool InternetMediaTypeRegistry::IsXMLFormat (const InternetMediaType& ct) const
{
    // @todo look into info in type files/backend to make extensible
    using AtomType = InternetMediaType::AtomType;
    if (ct.GetType<AtomType> () == InternetMediaTypes::Types::kApplication) {
        Assert (InternetMediaTypes::kXML.GetType<AtomType> () == InternetMediaTypes::Types::kApplication);
        if (ct.GetSubType<AtomType> () == InternetMediaTypes::kXML.GetSubType<AtomType> ()) {
            return true;
        }
        Assert (InternetMediaTypes::kXSLT.GetType<AtomType> () == InternetMediaTypes::Types::kApplication);
        if (ct.GetSubType<AtomType> () == InternetMediaTypes::kXSLT.GetSubType<AtomType> ()) {
            return true;
        }
    }
    if (ct.GetType<AtomType> () == InternetMediaTypes::Types::kText) {
        static const AtomType kXMLAtom_ = "xml"sv;
        if (ct.GetSubType<AtomType> () == kXMLAtom_) {
            return true;
        }
    }
    static const AtomType kXMLMediaTypeSuffix{"xml"sv};
    if (ct.GetSuffix<AtomType> () == kXMLMediaTypeSuffix) {
        return true;
    }
    /*
     * TODO:
     *      o   NEED EXTENSION MECHANSIM TO ADD OTHER TYPES
     */
    return false;
}

bool InternetMediaTypeRegistry::IsA (const InternetMediaType& moreGeneralType, const InternetMediaType& moreSpecificType) const
{
    /**
     *  Generally simple to compare because AtomType code and parser handle case and breaking off bits like +xml, and ; parameters
     * 
     *  Only trick is that no good way to tell more general relationships between types, but doesn't appear well defined (like CCR is a kind of XML).
     */
    using AtomType = InternetMediaType::AtomType;
    return moreSpecificType.GetType<AtomType> () == moreGeneralType.GetType<AtomType> () and
           moreSpecificType.GetSubType<AtomType> () == moreGeneralType.GetSubType<AtomType> ();
}
