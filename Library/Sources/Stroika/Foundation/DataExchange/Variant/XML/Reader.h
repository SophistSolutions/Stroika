/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_XML_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_XML_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/DataExchange/Variant/Reader.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Streams/InputStream.h"

#include "Binding.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::DataExchange::Variant::XML {

    /**
     *  \brief Read XML as a VariantValue - the inverse of Variant::XML::Writer, whose documentation has the shape
     *
     *  An element with child elements reads as an object, one with only text as that text, and children of one name as an
     *  array. **Every leaf reads as a String**: XML says nothing of types, and VariantValue::As (so ObjectVariantMapper's
     *  ToObject) coerces. An attribute reads as a member too, by its name, so reading a document of someone else's drops
     *  nothing silently - as does a Binding, which says which members are attributes, what they are named, and which
     *  are arrays however few elements a document happens to have.
     *
     *  So what the Writer wrote reads back with every leaf its As<String> (3 as "3", true as "true"), and an empty element as
     *  "" - which is what null, {} and an empty array of `<Array>`s are all written as. A Binding saying a member is an array
     *  (fItems naming its items) reads its empty element back as []; an empty array written as the member's element repeated
     *  is no element at all, so reads back absent.
     *
     *  Give it the Binding the Writer was given. With the default - no Binding::fName - the document element is the value's one
     *  member, so its name is kept: `<a><b>1</b></a>` reads as `{"a": {"b": "1"}}`. A Binding with an fName reads the document
     *  element's content as the value - and throws DataExchange::BadFormatException where that element has another (local)
     *  name, the document not being one the Binding describes.
     *
     *  \note Throws DataExchange::BadFormatException on mixed content (text beside child elements), which this shape cannot
     *        hold: reading on would silently drop the text. And on text beside attributes, unless a Binding says which member
     *        it is (Binding::Kind::eText).
     *
     *  \note Comments and processing instructions are dropped - StructuredStreamEvents::IConsumer reports neither, so this
     *        Reader never sees them. So a document rewritten through a VariantValue loses the comments it had, which would
     *        matter most for a hand-edited config file.
     *
     *        @todo keep comments and processing instructions, if ever wanted - easy enough here, but the SAX reader must
     *              report them first (a callback each on StructuredStreamEvents::IConsumer, emitted by both providers)
     *
     *  \note Requires an XML parser (qStroika_Foundation_DataExchange_XML_SupportParsing); Read throws without one.
     */
    class Reader : public Variant::Reader {
    private:
        using inherited = Variant::Reader;

    private:
        class Rep_;

    public:
        Reader (const Binding& binding = {});

    public:
        /**
         */
        nonvirtual Binding GetBinding () const;

    public:
        /**
         *  @see GetBinding
         */
        nonvirtual void SetBinding (const Binding& binding);

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
