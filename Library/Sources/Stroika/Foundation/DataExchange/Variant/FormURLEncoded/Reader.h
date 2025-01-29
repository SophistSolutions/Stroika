/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_FormURLEncoded_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_FormURLEncoded_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/DataExchange/Variant/Reader.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 */

namespace Stroika::Foundation::DataExchange::Variant::FormURLEncoded {

    using Characters::Character;
    using Characters::String;
    using Containers::Association;
    using Traversal::Iterable;

    /**
     *  \brief Read a stream into an Association (or VariantValue) - following https://url.spec.whatwg.org/#application/x-www-form-urlencoded
     * 
     *  \see InternetMediaTypes::kWWWFormURLEncoded
     * 
     *  \par Example Usage
     *      \code
     *         
     *      \endcode
     */
    class Reader : public Variant::Reader {
    private:
        using inherited = Variant::Reader;

    private:
        class Rep_;

    public:
        Reader ();

    public:
        /**
         *  Alternative reading API for this file type, which maybe helpful, to read as a series of lines.
         */
        nonvirtual Association<String, String> ReadAssociation (const Streams::InputStream::Ptr<byte>& in) const;
        nonvirtual Association<String, String> ReadAssociation (const Streams::InputStream::Ptr<Character>& in) const;
        nonvirtual Association<String, String> ReadAssociation (const Iterable<Character>& in);
        nonvirtual Association<String, String> ReadAssociation (istream& in);
        nonvirtual Association<String, String> ReadAssociation (wistream& in);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Reader.inl"

#endif /*_Stroika_Foundation_DataExchange_Variant_FormURLEncoded_Reader_h_*/
