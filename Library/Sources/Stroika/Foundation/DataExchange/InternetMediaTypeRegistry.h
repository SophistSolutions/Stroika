/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_
#define _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>
#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeNotSupportedException.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Memory/SharedByValue.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::DataExchange {

    using Characters::String;
    using Containers::Mapping;

    /**
     *  This leverages the os-dependent MIME databases
     *
     *  \todo Cleanup internally when we do caching and where and how. Not bad now - but maybe smarter todo at
     *        frontend level not backend level? Unclear (since some backends have different cost structures).
     *
     *        But sloppily done for now.
     *
     *  \par Example Usage
     *      \code
     *          if (InternetMediaTypeRegistry::sThe->IsTextFormat (InternetMediaType {some-string}) {
     *              handle_textfiles()
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          DbgTrace ("SUFFIX: {}"_f, InternetMediaTypeRegistry::sThe->GetPreferredAssociatedFileSuffix (i));
     *          DbgTrace ("ASSOCFILESUFFIXES: {}"_f, InternetMediaTypeRegistry::sThe->GetAssociatedFileSuffixes (i));
     *          DbgTrace ("GetAssociatedPrettyName: {}"_f, InternetMediaTypeRegistry::sThe->GetAssociatedPrettyName (i));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // updating media type registry, create a new one and call Set
     *          InternetMediaTypeRegistry origRegistry    = InternetMediaTypeRegistry::sThe.load ();
     *          InternetMediaTypeRegistry updatedRegistry = origRegistry;
     *          const auto                kHFType_        = InternetMediaType{"application/fake-heatlthframe-phr+xml"};
     *          EXPECT_TRUE (not InternetMediaTypeRegistry::sThe->GetMediaTypes ().Contains (kHFType_));
     *          updatedRegistry.AddOverride (kHFType_, InternetMediaTypeRegistry::OverrideRecord{nullopt, Set<String>{".HPHR"}, L".HPHR"});
     *          InternetMediaTypeRegistry::sThe.store (updatedRegistry);
     *          Assert (InternetMediaTypeRegistry::sThe->IsXMLFormat (kHFType_));
     *          Assert (InternetMediaTypeRegistry::sThe->GetMediaTypes ().Contains (kHFType_));
     *          Assert (not origRegistry.GetMediaTypes ().Contains (kHFType_));
     *          Assert (updatedRegistry.GetMediaTypes ().Contains (kHFType_));
     *      \endcode
     *
     *  TODO:
     *      \todo   http://stroika-bugs.sophists.com/browse/STK-714 - InternetMediaTypeRegistry Add mechanism to fetch subtypes more generally - enhance IsA
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    class InternetMediaTypeRegistry {
    public:
        struct IBackendRep;

    public:
        /**
         *  \brief - Generally no need to use this - handled automatically - but returns the default, OS-provided MIME InternetMediaType registry.
         *
         *  On Windows, this uses:
         *      HKEY_CLASSES_ROOT\MIME\Database\Content Type
         *
         *  On Linux/BSD (but not MacOS), this uses:
         *      /usr/share/mime/globs
         *      /etc/mime.types
         */
        static shared_ptr<IBackendRep> DefaultBackend ();

#if qPlatform_Windows
    public:
        /**
         *  Use:
         *      HKEY_CLASSES_ROOT\MIME\Database\Content Type
         */
        static shared_ptr<IBackendRep> WindowsRegistryDefaultBackend ();
#endif

    public:
        /**
         *  \brief - Generally no need to use this - handled automatically.
         *
         *  Available on:
         *      o   Linux
         *      o   BSD
         *
         *  /usr/share/mime/globs
         *
         *  This is the preferred backend on UNIX systems
         */
        static shared_ptr<IBackendRep> UsrSharedDefaultBackend ();

    public:
        /**
         *  \brief - Generally no need to use this - handled automatically.
         *
         *  Available on:
         *      o   Linux
         *      o   BSD
         *
         *  /etc/mime.types
         *
         *  This is not a very good choice, but will often work. It is fairly incomplete.
         */
        static shared_ptr<IBackendRep> EtcMimeTypesDefaultBackend ();

    public:
        /**
         *  \brief - Generally no need to use this - handled automatically.
         *
         *  Provides a handful of hardwired values - enough to pass regression tests, but not a good choice.
         *
         *  Available everywhere
         *
         *  This is a terrible choice, but better than nothing.
         */
        static shared_ptr<IBackendRep> BakedInDefaultBackend ();

    public:
        /**
         *  file suffix includes the dot; This COULD have been defined as a filesystem::path, as path::extension() returns path.
         *  But I think this is generally more convenient as a string and this class provides overloads when passing in an extension
         *  taking a filesystem::path.
         */
        using FileSuffixType = String;

    public:
        /**
         *  Used to override InternetMediaType file suffix/pretty name entries from the OS, or to define custom ones per-application.
         */
        struct OverrideRecord {
            optional<String>                          fTypePrintName;
            optional<Containers::Set<FileSuffixType>> fFileSuffixes;
            optional<FileSuffixType>                  fPreferredSuffix;
        };

    public:
        /**
         *  The default constructor makes a new (empty) copy of customizations, and uses DefaultBackend (). The constructor
         *  with the explicit backend, uses that backend.
         */
        InternetMediaTypeRegistry (const shared_ptr<IBackendRep>& backendRep = nullptr);
        InternetMediaTypeRegistry (const InternetMediaTypeRegistry& src) = default;

    public:
        nonvirtual InternetMediaTypeRegistry& operator= (InternetMediaTypeRegistry&& rhs)      = default;
        nonvirtual InternetMediaTypeRegistry& operator= (const InternetMediaTypeRegistry& rhs) = default;

    public:
        /**
         *  The current global variable - InternetMediaTypeRegistry. Typically - use this.
         * 
         *  \note copying InternetMediaTypeRegistry by value is cheap (shared-by-value) to avoiding the lock around sThe is easy - just copy the InternetMediaTypeRegistry::sThe.
         *
         *  \note  \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
         */
        static Execution::Synchronized<InternetMediaTypeRegistry> sThe;

    public:
        /**
         *  Return the current override mappings (note - these are initialized per-OS, to provide sometimes better values than that OS,
         *  but this can be overridden/cleared).
         */
        nonvirtual Mapping<InternetMediaType, OverrideRecord> GetOverrides () const;

    public:
        /**
         *  Set the current override mappings. Rarely called. More likely - call AddOverride()
         */
        nonvirtual void SetOverrides (const Mapping<InternetMediaType, OverrideRecord>& overrides);

    public:
        /**
         *  Typically used to add custom internet media type mappings to file names. But can be used to override operating system defaults.
         */
        nonvirtual void AddOverride (const InternetMediaType& mediaType, const OverrideRecord& overrideRec);

    private:
        struct IFrontendRep_;
        struct FrontendRep_;

        struct Rep_Cloner_ {
            shared_ptr<IFrontendRep_> operator() (const IFrontendRep_& t) const;
        };
        using SharedRepByValuePtr_ =
            Memory::SharedByValue<IFrontendRep_, Memory::SharedByValue_Traits<IFrontendRep_, shared_ptr<IFrontendRep_>, Rep_Cloner_>>;

        SharedRepByValuePtr_ fFrontEndRep_;

    public:
        /**
         */
        nonvirtual Containers::Set<InternetMediaType> GetMediaTypes () const;
        nonvirtual Containers::Set<InternetMediaType> GetMediaTypes (InternetMediaType::AtomType majorType) const;

    public:
        /**
         *  There are frequently many file suffixes associated with a given filetype. This routine fetches the single best/preferred value.
         */
        nonvirtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const;

    public:
        /**
         *  There can be more than one file suffix associated with a content type.
         */
        nonvirtual Containers::Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const;
        nonvirtual Containers::Set<FileSuffixType> GetAssociatedFileSuffixes (const Traversal::Iterable<InternetMediaType>& cts) const;

    public:
        /**
         * return nullopt if not found
         */
        nonvirtual optional<String> GetAssociatedPrettyName (const InternetMediaType& ct) const;

    public:
        /**
         * return nullopt if not found
         *
         *  \note **Since Stroika v2.1b2 - this takes fileSuffix argument - NOT fileNameOrSuffix - because path::extension ()
         *
         *  \req fileSuffix.empty () or fileSuffix[0] == '.'
         */
        nonvirtual optional<InternetMediaType> GetAssociatedContentType (const FileSuffixType& fileSuffix) const;
        nonvirtual optional<InternetMediaType> GetAssociatedContentType (const filesystem::path& fileSuffix) const;

    public:
        /**
         *  \brief returns true if you can expect to treat as some sort of text and reasonably view - like text/html, application/x-ccr, application/x-url, etc...
         *
         *  This examines the 'Type' field, sometimes subtype field, as well as leverages the Suffix field (if present).
         */
        nonvirtual bool IsTextFormat (const InternetMediaType& ct) const;

    public:
        /**
         * This returns true if the given type is known to be treatable as an image. 
         */
        nonvirtual bool IsImageFormat (const InternetMediaType& ct) const;

    public:
        /**
         * This returns true if the given type is known to be treatable as an XML. 
         *
         *  This examines the 'Type' field, sometimes subtype field, as well as leverages the Suffix field (if present).
         *
         *  \note This returns true if 'ct.GetSuffix () == "xml"
         */
        nonvirtual bool IsXMLFormat (const InternetMediaType& ct) const;

    public:
        /**
         *  \brief return true if moreSpecificType 'isa' moreGeneralType
         *
         *  The HISTORICAL algorithm for this is:
         *
         *         This function compares similar types, like 
         *         application/healthframe-PHR-Format and
         *         application/healthframe-PHR-Format-2 etc
         *         and returns true iff the given type is a prefix (case insensitive)
         *         of the argument more general one. The types must match, and the
         *         parameters are ignored.
         *
         *         Change as of Stroika v2.1d27 - now only checks prefix of
         *         subtype - type must match - and now ignores parameters.
         *
         *  @todo REDO this - and dont count on above old algorith. Will add new mechanism EITHER based on what I can read from
         *        the MIME config files on each OS (except it appears windows), or from some registration;
         *        see http://stroika-bugs.sophists.com/browse/STK-714
         * 
         *  As of Stroika v3.0d5 - this just checks the type and subtype atoms for equality (which are already case insensitive) and 
         *  ignores file suffix and parameters.
         * 
         *  However, unclear how to capture semantic things like XML is a kind of text, or CCR is a kind of XML. And certainly nothing like the -1, -2 stuff
         *  above with HELATHFRAME PHR format.
         */
        nonvirtual bool IsA (const InternetMediaType& moreGeneralType, const InternetMediaType& moreSpecificType) const;

    public:
        /** 
         */
        template <typename EXCEPTION = InternetMediaTypeNotSupportedException>
        nonvirtual void CheckIsA (const InternetMediaType& moreGeneralType, const InternetMediaType& moreSpecificType,
                                  const EXCEPTION& throwIfNot = InternetMediaTypeNotSupportedException::kThe) const;

    public:
        [[deprecated ("Since Stroika v3.0d10 - just access sThe->")]] static InternetMediaTypeRegistry Get ()
        {
            return sThe.load ();
        }
        [[deprecated ("Since Stroika v3.0d10 - just set sThe")]] static void Set (const InternetMediaTypeRegistry& newRegistry)
        {
            sThe = newRegistry;
        }
    };
    inline Execution::Synchronized<InternetMediaTypeRegistry> InternetMediaTypeRegistry::sThe;

    /**
     */
    struct InternetMediaTypeRegistry::IBackendRep {
        virtual ~IBackendRep ()                                                                                            = default;
        virtual Containers::Set<InternetMediaType> GetMediaTypes (optional<InternetMediaType::AtomType> majorType) const   = 0;
        virtual optional<FileSuffixType>           GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const    = 0;
        virtual Containers::Set<FileSuffixType>    GetAssociatedFileSuffixes (const InternetMediaType& ct) const           = 0;
        virtual optional<String>                   GetAssociatedPrettyName (const InternetMediaType& ct) const             = 0;
        virtual optional<InternetMediaType>        GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const = 0;
    };

    /**
     */
    struct InternetMediaTypeRegistry::IFrontendRep_ {
        virtual ~IFrontendRep_ ()                                                = default;
        virtual Mapping<InternetMediaType, OverrideRecord> GetOverrides () const = 0;
        virtual void                                       SetOverrides (const Mapping<InternetMediaType, OverrideRecord>& overrides)  = 0;
        virtual void                               AddOverride (const InternetMediaType& mediaType, const OverrideRecord& overrideRec) = 0;
        virtual shared_ptr<IBackendRep>            GetBackendRep () const                                                              = 0;
        virtual Containers::Set<InternetMediaType> GetMediaTypes (optional<InternetMediaType::AtomType> majorType) const               = 0;
        virtual optional<FileSuffixType>           GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const                = 0;
        virtual Containers::Set<FileSuffixType>    GetAssociatedFileSuffixes (const InternetMediaType& ct) const                       = 0;
        virtual optional<String>                   GetAssociatedPrettyName (const InternetMediaType& ct) const                         = 0;
        virtual optional<InternetMediaType>        GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const             = 0;
    };

    /**
     *  \note
     *      @see http://www.iana.org/assignments/media-types/media-types.xhtml
     */

    /**
     *  The currently registered types are: 
     *          application, audio, example, font, image, message, model, multipart, text and video
     *
     *  \note Types - here - refers to MAJOR types - not InternetMediaTypes (so just top level stuff before the /)
     */
    namespace InternetMediaTypes::Types {

        /**
         *  \brief 'application'
         *
         *  This is the major type (atom) making up a class of InternetMediaTypes.
         */
        inline const InternetMediaType::AtomType kApplication{"application"sv};

        /**
         *  \brief 'image'
         *
         *  This is the major type (atom) making up a class of InternetMediaTypes.
         */
        inline const InternetMediaType::AtomType kImage{"image"sv};

        /**
         *  \brief 'text'
         *
         *  This is the major type (atom) making up a class of InternetMediaTypes.
         */
        inline const InternetMediaType::AtomType kText{"text"sv};

    }

    namespace InternetMediaTypes {

        /**
         *  \brief application/octet-stream
         */
        inline const InternetMediaType kOctetStream{Types::kApplication, "octet-stream"sv};

        /**
         *  \brief image/png
         */
        inline const InternetMediaType kPNG{Types::kImage, "png"sv};

        /**
         *  \brief image/gif
         */
        inline const InternetMediaType kGIF{Types::kImage, "gif"sv};

        /**
         *  \brief image/jpeg
         */
        inline const InternetMediaType kJPEG{Types::kImage, "jpeg"sv};

        /**
         * \brief text/html
         */
        inline const InternetMediaType kHTML{Types::kText, "html"sv};

        /**
         * \brief text/javascript
         */
        inline const InternetMediaType kJavascript{Types::kText, "javascript"sv};

        /**
         * \brief text/css
         */
        inline const InternetMediaType kCSS{Types::kText, "css"sv};

        /**
         *  \brief text/plain
         */
        inline const InternetMediaType kText_PLAIN{Types::kText, "plain"sv};

        /**
         *  \brief text/cvs
         */
        inline const InternetMediaType kCSV{Types::kText, "csv"sv};

        /**
         *  \brief application/json
         */
        inline const InternetMediaType kJSON{Types::kApplication, "json"sv};

        /**
         *  \brief application/pdf
         */
        inline const InternetMediaType kPDF{Types::kApplication, "pdf"sv};

        /**
         * \brief text/uri-list (@see https://tools.ietf.org/html/rfc2483#section-5)
         *
         *  \note until Stroika 2.1b2, this was defined as application/x-url
         */
        inline const InternetMediaType kURL{Types::kText, "uri-list"sv};

        /**
         *  \brief application/xml (@see https://www.w3.org/2006/02/son-of-3023/draft-murata-kohn-lilley-xml-04.html#rfc.section.3.2)
         *
         *  \note use this because text/xml deprecated - https://www.w3.org/2006/02/son-of-3023/draft-murata-kohn-lilley-xml-04.html#:~:text=Text%2Fxml%20Registration%20(deprecated),-MIME%20media%20type&text=Although%20listed%20as%20an%20optional,based%20content%20negotiation%20in%20HTTP.
         */
        inline const InternetMediaType kXML{Types::kApplication, "xml"sv};

        /**
         *  \brief application/x-xslt
         */
        inline const InternetMediaType kXSLT{Types::kApplication, "x-xslt"sv};

        /**
         */
        inline const InternetMediaType kJavaArchive{Types::kApplication, "java-archive"sv};

        /**
         * Microsoft RTF - Rich Text Format
         */
        inline const InternetMediaType kRTF{Types::kApplication, "rtf"sv};

        /**
         *  \brief application/zip
         */
        inline const InternetMediaType kZip{Types::kApplication, "zip"sv};

    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetMediaTypeRegistry.inl"

#endif /*_Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_*/
