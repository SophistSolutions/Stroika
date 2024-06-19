/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_ContentCoding_h_
#define _Stroika_Foundation_IO_Network_HTTP_ContentCoding_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/Atom.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;

    /**
     *  \brief Content coding values indicate an encoding transformation that has been or can be applied to an entity. Content codings are primarily used to allow a document to be compressed or otherwise usefully transformed without losing the identity of its underlying media type and without loss of information. Frequently, the entity is stored in coded form, transmitted directly, and only decoded by the recipient.
     * 
     *  \note   Configuration::DefaultNames<> supported
     */
    struct ContentCoding {
    public:
        /**
         */
        using AtomType = DataExchange::Atom<DataExchange::AtomManager_CaseInsensitive>;

    public:
        /**
         */
        ContentCoding (AtomType a);
        template <Characters::IConvertibleToString STRING_LIKE>
        ContentCoding (STRING_LIKE&& name);

    private:
        AtomType fRep_;

    public:
        /**
         */
        static const ContentCoding kCompress;
        /**
         */
        static const ContentCoding kDeflate;
        /**
         */
        static const ContentCoding kGZip;
        /**
         */
        static const ContentCoding kIdentity;
    };
    const inline ContentCoding ContentCoding::kCompress{"compress"sv};
    const inline ContentCoding ContentCoding::kDeflate{"deflate"sv};
    const inline ContentCoding ContentCoding::kGZip{"gzip"sv};
    const inline ContentCoding ContentCoding::kIdentity{"identity"sv};

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ContentCoding.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_ContentCoding_h_*/
