/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_
#define _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_ 1

#include "../StroikaPreComp.h"

#include <filesystem>
#include <optional>

#include "../Characters/String.h"
#include "../Containers/Set.h"

#include "InternetMediaType.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::DataExchange {

    using Characters::String;
    using Containers::Set;

    /**
     *  This leverages the os-dependent MIME databases
     *
     *  @todo maybe virtualize interface and provide other implementations (which is why we have Default () API).
     *        and much more - see https://stroika.atlassian.net/browse/STK-576
     *
     *  \todo Cleanup internally when we do caching and where and how. Not bad now - but maybe smarter todo at
     *        frontend level not backend level? Unclear (since some backends have different cost structures).
     *
     *        But sloppily done for now.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Rep-Inside-Ptr-Is-Internally-Synchronized">Rep-Inside-Ptr-Is-Internally-Synchronized</a>
     *
     */
    class InternetMediaTypeRegistry {
    public:
        struct IBackendRep;

    public:
        /**
         *  Return the default, OS-provided MIME InternetMediaType registry.
         *
         *  On Windows, this uses:
         *      HKEY_CLASSES_ROOT\MIME\Database\Content Type
         *
         *  On Linux/BSD (but not MacOS), this uses:
         *      /usr/share/mime/globs
         *      /etc/mime.types
         *
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
         *  Provides a handfull of hardwired values - enough to pass regression tests, but not a good choice.
         *
         *  Available everywhere
         *
         *  This is a terrible choice, but better than nothing.
         */
        static shared_ptr<IBackendRep> BakedInDefaultBackend ();

    public:
        /**
         *  Return the current global variable - current internet media type registry. Typically - use this.
         *
         *  \\todo @todo Consider adding a Set() method, that would allow replacing the global internet media content-type registry
         *         But - in the meantime, these can be copied, and separately constructed, and used explicitly in any context where
         *          a user wants a specific registry.
         */
    public:
        static const InternetMediaTypeRegistry& Get ();

    public:
        // DEPRECATED
        [[deprecated ("Since Stroika 2.1b2 - use sThe (?) instead")]] static InternetMediaTypeRegistry Default ()
        {
            return Get ();
        }

    public:
        /**
         *  The default constructor makes a new (empty) copy of customizations, and uses DefaultBackend (). The constructor
         *  with the explicit backend, uses that backend.
         */
        InternetMediaTypeRegistry (const shared_ptr<IBackendRep>& backendRep = nullptr);

    private:
        struct IFrontendRep_;
        struct FrontendRep_;
        shared_ptr<IFrontendRep_> fFrontEndRep_;

    public:
        /**
         *  file suffix includes the dot; This COULD have been defined as a filesystem::path, as path::extension() returns path.
         *  But I think this is generally more convenient as a string and this class provides overloads when passing in an extension
         *  taking a filesystem::path.
         */
        using FileSuffixType = String;

    public:
        /**
         */
        nonvirtual Set<InternetMediaType> GetMediaTypes () const;
        nonvirtual Set<InternetMediaType> GetMediaTypes (InternetMediaType::AtomType majorType) const;

    public:
        /**
         *  There are frequently many file suffixes associated with a given filetype. This routine fetches the single best/preferred value.
         */
        nonvirtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const;

    public:
        /**
         *  There can be more than one file suffix associated with a content type. Note that this returns file
         *  suffixes from more general InternetMediaTypes as well as the given one.
         */
        nonvirtual Set<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct) const;
        nonvirtual Set<FileSuffixType> GetAssociatedFileSuffixes (const Traversal::Iterable<InternetMediaType>& cts) const;

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
         */
        nonvirtual bool IsXMLFormat (const InternetMediaType& ct) const;
    };

    /**
     */
    struct InternetMediaTypeRegistry::IBackendRep {
        virtual ~IBackendRep ()                                                                                     = default;
        virtual Set<InternetMediaType>      GetMediaTypes (optional<InternetMediaType::AtomType> majorType) const   = 0;
        virtual optional<FileSuffixType>    GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const    = 0;
        virtual Set<FileSuffixType>         GetAssociatedFileSuffixes (const InternetMediaType& ct) const           = 0;
        virtual optional<String>            GetAssociatedPrettyName (const InternetMediaType& ct) const             = 0;
        virtual optional<InternetMediaType> GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const = 0;
    };

    /**
     */
    struct InternetMediaTypeRegistry::IFrontendRep_ {
        virtual ~IFrontendRep_ ()                                                                                   = default;
        virtual Set<InternetMediaType>      GetMediaTypes (optional<InternetMediaType::AtomType> majorType) const   = 0;
        virtual optional<FileSuffixType>    GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const    = 0;
        virtual Set<FileSuffixType>         GetAssociatedFileSuffixes (const InternetMediaType& ct) const           = 0;
        virtual optional<String>            GetAssociatedPrettyName (const InternetMediaType& ct) const             = 0;
        virtual optional<InternetMediaType> GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetMediaTypeRegistry.inl"

#endif /*_Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_*/
