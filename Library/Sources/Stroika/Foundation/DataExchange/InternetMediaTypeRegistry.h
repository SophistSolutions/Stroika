/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_
#define _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_ 1

#include "../StroikaPreComp.h"

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

     @todo use Stroika cache code internally - internallysyncrhonized cache

     @todo DOCUMENT/ASSURE INTERNALLY SYNCRHONIZED
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
         *  Return the current global variable - current internet media type registry. Typically - use this.

         **** IN FUTURE VERISONS - THIS WILL BE CUSTOMIZABLE/EDITABLE, or copy copyable to a private copy you can edit/use

         **** unclear if we want to allow assignemnt - maybe should have Get/Set methods
         */
    public:
        static InternetMediaTypeRegistry sThe;

    public:
        // DEPRECATED
        [[deprecated ("Since Stroika 2.1b2 - use sThe (?) instead")]] static InternetMediaTypeRegistry Default ()
        {
            return sThe;
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
         *  file suffix includes the dot
         */
        using FileSuffixType = String;

    public:
        /**
         *  There are frequently many file suffixes associated with a given filetype. This routine fetches the single best/preferred value.
         */
        nonvirtual optional<FileSuffixType> GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const;

    public:
        /**
         *  Some types can be more general or more specific versions of a given type.
         *  Return those more general (including argument).
         */
        nonvirtual Set<InternetMediaType> GetMoreGeneralTypes (const InternetMediaType& ct) const;

    public:
        /**
         *  Some types can be more general or more specific versions of a given type.
         *  Return those more specific (including argument).
         */
        nonvirtual Set<InternetMediaType> GetMoreSpecificTypes (const InternetMediaType& ct) const;

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
         */
        nonvirtual optional<InternetMediaType> GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const;
    };

    /**
     */
    struct InternetMediaTypeRegistry::IBackendRep {
        virtual ~IBackendRep ()                                                                                     = default;
        virtual optional<FileSuffixType>    GetPreferredAssociatedFileSuffix (const InternetMediaType& ct) const    = 0;
        virtual Set<FileSuffixType>         GetAssociatedFileSuffixes (const InternetMediaType& ct) const           = 0;
        virtual optional<String>            GetAssociatedPrettyName (const InternetMediaType& ct) const             = 0;
        virtual optional<InternetMediaType> GetAssociatedContentType (const FileSuffixType& fileNameOrSuffix) const = 0;
    };

    /**
     */
    struct InternetMediaTypeRegistry::IFrontendRep_ {
        virtual ~IFrontendRep_ ()                                                                                   = default;
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
