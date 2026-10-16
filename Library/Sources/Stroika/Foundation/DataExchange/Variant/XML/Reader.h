/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_XML_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_XML_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/DataExchange/Variant/Reader.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/DataExchange/XML/Providers/IProvider.h"
#include "Stroika/Foundation/DataExchange/XML/Schema.h"
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
     *  array. An attribute reads as a member too, by its name, so reading a document of someone else's drops nothing
     *  silently. A Binding says the rest: which members are attributes, what they are named, which are arrays however few
     *  elements a document has - and (Binding::fType) what type each value is.
     *
     *  **Whether the Binding declares a value's type changes how it reads, a great deal** (Binding::fType's table):
     *      - **No type declared** - the default - XML says nothing of one, so this guesses: **a leaf reads back as a String**
     *        (3 as "3", true as "true" - VariantValue::As, so ObjectVariantMapper's ToObject, coerces), an empty element as ""
     *        (which is what null, {} and an empty array of `<Array>`s are all written as), and an element it does not name as a
     *        member, by its name. A Binding saying a member is an array (fItems naming its items) reads its empty element back
     *        as []; an empty array written as the member's element repeated is no element at all, so reads back absent.
     *      - **A type declared**: the value reads back as that type - 3 as 3, an empty object as {}, an empty array as [] - and
     *        a document that is not of that type (text that is not a number, an element or attribute the Binding does not
     *        declare, a member repeated that it declares as one value) throws BadFormatException - unless the Binding says to
     *        skip what it does not declare (Binding::fUndeclaredMembers eSkip, as XSD's xs:any), as reading someone else's
     *        document - a vendor's own elements - needs.
     *
     *  **Given a complete Binding (Binding::IsComplete), what the Writer wrote reads back exactly - or the Writer, or this
     *  Reader, throws BadFormatException, the value not being one the Binding describes: never anything in between** (but for
     *  what XML writes as nothing - @see Binding::IsComplete).
     *
     *  Give it (Options::fBinding) the Binding the Writer was given. With the default - no Binding::fName - the document
     *  element is the value's one member, so its name is kept: `<a><b>1</b></a>` reads as `{"a": {"b": "1"}}`. A Binding with
     *  an fName reads the document element's content as the value - and throws DataExchange::BadFormatException where that
     *  element has another (local) name, the document not being one the Binding describes.
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
        /**
         *  \brief How to read: the Binding - and which parser reads, and a schema the document must hold to
         */
        struct Options {
            /**
             *  \brief The Binding the Writer was given (@see Binding)
             */
            Binding fBinding;

            /**
             *  \brief The provider whose parser reads - nullptr for Stroika's default. A SAX parser is all the Reader needs
             */
            const DataExchange::XML::Providers::ISAXProvider* fXMLProvider{nullptr};

            /**
             *  \brief A schema the document is validated against as it is read: reading one that does not hold to it throws.
             *         nullptr (the default) validates nothing
             *
             *  Best compiled by the provider that reads (fXMLProvider) - @see DataExchange::XML::SAXParse.
             */
            DataExchange::XML::Schema::Ptr fValidateAgainst{nullptr};
        };

    public:
        /**
         *  \note Two constructors, not one with a default argument (= {}): within Reader, that default argument cannot use the
         *        member initializers of Options, nested in it (C++ CWG 1397) - MSVC allows it, clang and gcc do not.
         */
        Reader ();
        Reader (const Options& options); ///< \brief Reading as options says
        Reader (const Binding& binding); ///< \brief As Reader{Options{.fBinding = binding}}
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Variant_XML_Reader_h_*/
