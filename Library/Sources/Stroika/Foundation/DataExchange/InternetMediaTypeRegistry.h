/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_
#define _Stroika_Foundation_DataExchange_InternetMediaTypeRegistry_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../Characters/String.h"

#include "InternetMediaType.h"

/**
 */

namespace Stroika::Foundation::DataExchange {

    using Characters::String;
    using Containers::Sequence;

    /**
     *  This leverages the os-dependent mime/databases
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
         */
        nonvirtual Sequence<FileSuffixType> GetAssociatedFileSuffixes (const InternetMediaType& ct, bool includeMoreGeneralTypes = true) const;

    public:
        /**
         */
        nonvirtual optional<String> GetAssociatedPrettyName (const InternetMediaType& ct) const;

    public:
        /**
         * return empty string if not found
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
