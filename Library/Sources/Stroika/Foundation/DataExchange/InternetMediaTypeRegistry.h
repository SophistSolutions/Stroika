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
     *
     */
    class InternetMediaTypeRegistry {
    private:
        InternetMediaTypeRegistry () = default;

    public:
        /**
         *  Return the default, OS-provided MIME InternetMediaType registry.
         *
         *  On Windows, this uses:
         *      HKEY_CLASSES_ROOT\MIME\Database\Content Type
         *
         *  On Linux, this uses:
         *      /usr/share/mime files
         *
         */
        static const InternetMediaTypeRegistry Default ();

    public:
        /**
         *  file suffix includes the dot
         */
        using FileSuffixType = String;

    public:
        /**
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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetMediaTypeRegistry.inl"

#endif /*_Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_*/
