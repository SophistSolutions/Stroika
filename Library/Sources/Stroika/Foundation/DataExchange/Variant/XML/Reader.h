/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_XML_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_XML_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/DataExchange/Variant/Reader.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/DataExchange/XML/SerializationConfiguration.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   NotYetImplemented
 *
 *      @todo   fix thread-safety - cloning rep - lock for access config data
 */

namespace Stroika::Foundation::DataExchange::Variant::XML {

    using DataExchange::XML::SerializationConfiguration;

    /**
     *  \brief Read XML as a VariantValue - the inverse of Variant::XML::Writer, whose documentation has the shape
     *
     *  An element with child elements reads as an object, one with only text as that text, and children of one name as an
     *  array. **Every leaf reads as a String**: XML says nothing of types, and VariantValue::As (so ObjectVariantMapper's
     *  ToObject) coerces. An attribute reads as a member too, by its name, so reading a document of someone else's drops
     *  nothing silently - as does an XML::Binding, which says which members are attributes, what they are named, and which
     *  are arrays however few elements a document happens to have.
     *
     *  \note Throws DataExchange::BadFormatException on mixed content (text beside child elements), which this shape cannot
     *        hold: reading on would silently drop the text.
     *
     *  \note Comments and processing instructions are dropped: SAX (StructuredStreamEvents::IConsumer) does not report them,
     *        so they cannot be read at all. @see https://github.com/SophistSolutions/Stroika/issues/954
     *
     *  \note Requires an XML parser (qStroika_Foundation_DataExchange_XML_SupportParsing); Read throws without one.
     */
    class Reader : public Variant::Reader {
    private:
        using inherited = Variant::Reader;

    private:
        class Rep_;

    public:
        Reader (const SerializationConfiguration& config = {});

    public:
        nonvirtual SerializationConfiguration GetConfiguration () const;
        nonvirtual void                       SetConfiguration (const SerializationConfiguration& config);

    private:
        nonvirtual const Rep_& GetRep_ () const;
        nonvirtual Rep_&       GetRep_ ();
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Variant_XML_Reader_h_*/
