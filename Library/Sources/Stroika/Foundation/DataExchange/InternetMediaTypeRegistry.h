/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_
#define _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_ 1

#include "../StroikaPreComp.h"

#include <filesystem>
#include <optional>

#include "../Characters/String.h"
#include "../Containers/Mapping.h"
#include "../Containers/Set.h"
#include "../Memory/SharedByValue.h"

#include "InternetMediaType.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::DataExchange {

    using Characters::String;
    using Containers::Mapping;
    using Containers::Set;

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
     *          if (InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaType {somestring}) {
     *              handle_textfiles()
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          DbgTrace (L"SUFFIX: %s", Characters::ToString (InternetMediaTypeRegistry::Get ().GetPreferredAssociatedFileSuffix (i)).c_str ());
     *          DbgTrace (L"ASSOCFILESUFFIXES: %s", Characters::ToString (InternetMediaTypeRegistry::Get ().GetAssociatedFileSuffixes (i)).c_str ());
     *          DbgTrace (L"GetAssociatedPrettyName: %s", Characters::ToString (InternetMediaTypeRegistry::Get ().GetAssociatedPrettyName (i)).c_str ());
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // updating media type registry, create a new one and call Set
     *          InternetMediaTypeRegistry origRegistry    = InternetMediaTypeRegistry::Get ();
     *          InternetMediaTypeRegistry updatedRegistry = origRegistry;
     *          const auto                kHFType_        = InternetMediaType{L"application/fake-heatlthframe-phr+xml"};
     *          VerifyTestResult (not InternetMediaTypeRegistry::Get ().GetMediaTypes ().Contains (kHFType_));
     *          updatedRegistry.AddOverride (kHFType_, InternetMediaTypeRegistry::OverrideRecord{nullopt, Set<String>{L".HPHR"}, L".HPHR"});
     *          InternetMediaTypeRegistry::Set (updatedRegistry);
     *          Assert (InternetMediaTypeRegistry::Get ().IsXMLFormat (kHFType_));
     *          Assert (InternetMediaTypeRegistry::Get ().GetMediaTypes ().Contains (kHFType_));
     *          Assert (not origRegistry.GetMediaTypes ().Contains (kHFType_));
     *          Assert (updatedRegistry.GetMediaTypes ().Contains (kHFType_));
     *      \endcode
     *
     *  TODO:
     *      \todo   https://stroika.atlassian.net/browse/STK-714 - InternetMediaTypeRegistry Add mechanism to fetch subtypes more generally - enhance IsA
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
         *  This is the prefered backend on UNIX systems
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
         *  Provides a handfull of hardwired values - enough to pass regression tests, but not a good choice.
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
            optional<String>              fTypePrintName;
            optional<Set<FileSuffixType>> fFileSuffixes;
            optional<FileSuffixType>      fPreferredSuffix;
        };

    public:
        /**
         *  The default constructor makes a new (empty) copy of customizations, and uses DefaultBackend (). The constructor
         *  with the explicit backend, uses that backend.
         */
        InternetMediaTypeRegistry (const shared_ptr<IBackendRep>& backendRep = nullptr);
        InternetMediaTypeRegistry (const InternetMediaTypeRegistry& src) = default;

    public:
        nonvirtual InternetMediaTypeRegistry& operator= (const InternetMediaTypeRegistry& rhs) = default;

    public:
        /**
         *  Return the current global variable - current internet media type registry. Typically - use this.
         *
         *  \see Set ()
         *
         *  \note  \em Thread-Safety   <a href="Thread-Safety.md#Rep-Inside-Ptr-Is-Internally-Synchronized">Rep-Inside-Ptr-Is-Internally-Synchronized</a>
         */
    public:
        static InternetMediaTypeRegistry Get ();

    public:
        /**
         *  Set the current global variable - current internet media type registry. Typically - use this.
         *
         *  \see Get ()
         *
         *  \note  \em Thread-Safety   <a href="Thread-Safety.md#Rep-Inside-Ptr-Is-Internally-Synchronized">Rep-Inside-Ptr-Is-Internally-Synchronized</a>
         */
        static void Set (const InternetMediaTypeRegistry& newRegistry);

    public:
        // DEPRECATED
        [[deprecated ("Since Stroika 2.1b2 - use sThe (?) instead")]] static InternetMediaTypeRegistry Default ()
        {
            return Get ();
        }

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

        struct _Rep_Cloner {
            shared_ptr<IFrontendRep_> operator() (const IFrontendRep_& t) const;
        };
        using SharedRepByValuePtr_ = Memory::SharedByValue<IFrontendRep_, Memory::SharedByValue_Traits<IFrontendRep_, shared_ptr<IFrontendRep_>, _Rep_Cloner>>;

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
         *  \note This returns true if 'ct.GetSuffix () == L"xml"
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
         *        see https://stroika.atlassian.net/browse/STK-714
         *        
         */
        nonvirtual bool IsA (const InternetMediaType& moreGeneralType, const InternetMediaType& moreSpecificType) const;
    };

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
        virtual ~IFrontendRep_ ()                                                                                                              = default;
        virtual Mapping<InternetMediaType, OverrideRecord> GetOverrides () const                                                               = 0;
        virtual void                                       SetOverrides (const Mapping<InternetMediaType, OverrideRecord>& overrides)          = 0;
        virtual void                                       AddOverride (const InternetMediaType& mediaType, const OverrideRecord& overrideRec) = 0;
        virtual shared_ptr<IBackendRep>                    GetBackendRep () const                                                              = 0;
        virtual Containers::Set<InternetMediaType>         GetMediaTypes (optional<InternetMediaType::AtomType> majorType) const               = 0;
        virtual optional<FileSuffixType>                   GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const                = 0;
        virtual Containers::Set<FileSuffixType>            GetAssociatedFileSuffixes (const InternetMediaType& ct) const                       = 0;
        virtual optional<String>                           GetAssociatedPrettyName (const InternetMediaType& ct) const                         = 0;
        virtual optional<InternetMediaType>                GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const             = 0;
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
        inline const InternetMediaType::AtomType kApplication{L"application"sv};

        /**
         *  \brief 'image'
         *
         *  This is the major type (atom) making up a class of InternetMediaTypes.
         */
        inline const InternetMediaType::AtomType kImage{L"image"sv};

        /**
         *  \brief 'text'
         *
         *  This is the major type (atom) making up a class of InternetMediaTypes.
         */
        inline const InternetMediaType::AtomType kText{L"text"sv};

    }

    namespace InternetMediaTypes {

        /**
         *  \brief application/octet-stream
         */
        inline const InternetMediaType kOctetStream{Types::kApplication, L"octet-stream"sv};

        /**
         *  \brief image/png
         */
        inline const InternetMediaType kPNG{Types::kImage, L"png"sv};

        /**
         *  \brief image/gif
         */
        inline const InternetMediaType kGIF{Types::kImage, L"gif"sv};

        /**
         *  \brief image/jpeg
         */
        inline const InternetMediaType kJPEG{Types::kImage, L"jpeg"sv};

        /**
         * \brief text/html
         */
        inline const InternetMediaType kHTML{Types::kText, L"html"sv};

        /**
         * \brief text/css
         */
        inline const InternetMediaType kCSS{Types::kText, L"css"sv};

        /**
         *  \brief text/plain
         */
        inline const InternetMediaType kText_PLAIN{Types::kText, L"plain"sv};

        /**
         *  \brief text/cvs
         */
        inline const InternetMediaType kCSV{Types::kText, L"csv"sv};

        /**
         *  \brief application/json
         */
        inline const InternetMediaType kJSON{Types::kApplication, L"json"sv};

        /**
         *  \brief application/pdf
         */
        inline const InternetMediaType kPDF{Types::kApplication, L"pdf"sv};

        /**
         * \brief text/uri-list (@see https://tools.ietf.org/html/rfc2483#section-5)
         *
         *  \note until Stroika 2.1b2, this was defined as application/x-url
         */
        inline const InternetMediaType kURL{Types::kText, L"uri-list"sv};

        /**
         *  \brief application/xml (@see https://www.w3.org/2006/02/son-of-3023/draft-murata-kohn-lilley-xml-04.html#rfc.section.3.2)
         *
         *  \note use this because text/xml deprecated - https://www.w3.org/2006/02/son-of-3023/draft-murata-kohn-lilley-xml-04.html#:~:text=Text%2Fxml%20Registration%20(deprecated),-MIME%20media%20type&text=Although%20listed%20as%20an%20optional,based%20content%20negotiation%20in%20HTTP.
         */
        inline const InternetMediaType kXML{Types::kApplication, L"xml"sv};

        /**
         *  \brief application/x-xslt
         */
        inline const InternetMediaType kXSLT{Types::kApplication, L"x-xslt"sv};

        /**
         */
        inline const InternetMediaType kJavaArchive{Types::kApplication, L"java-archive"sv};

        /**
         * Microsoft RTF - Rich Text Format
         */
        inline const InternetMediaType kRTF{Types::kApplication, L"rtf"sv};

        /**
         *  \brief application/zip
         */
        inline const InternetMediaType kZip{Types::kApplication, L"zip"sv};

    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetMediaTypeRegistry.inl"

#endif /*_Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_*/
