/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_XML_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_XML_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/DataExchange/Variant/Reader.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/DataExchange/XML/Options.h"
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
     *        document - a vendor's own elements - needs; or the Reader reads leniently (Options::fLenient), passing over
     *        all of these.
     *
     *  **Given a complete Binding (Binding::IsComplete), this reads back a value equal to what the Writer wrote
     *  (VariantValue::operator==: the same members, items and types, and Strings of the same code points) - or the Writer, or
     *  this Reader, throws BadFormatException, the value not being one the Binding describes: never anything in between** (but
     *  for what XML writes as nothing - @see Binding::IsComplete). The document is not promised: the Writer picks one spelling
     *  of each character - the character itself, or a character reference - and any other spelling reads the same (U+1F600
     *  written literally, as `&#128512;` or as `&#x1F600;` is one character). A character XML 1.0 cannot hold at all (U+0000
     *  to U+001F but TAB, LF and CR, U+FFFE, U+FFFF), the Writer throws on.
     *
     *  Give it (Options::fBinding) the Binding the Writer was given. With the default - no Binding::fName - the document
     *  element is the value's one member, so its name is kept: `<a><b>1</b></a>` reads as `{"a": {"b": "1"}}`. A Binding with
     *  an fName reads the document element's content as the value - and throws DataExchange::BadFormatException where that
     *  element has another (local) name, the document not being one the Binding describes.
     *
     *  \note Throws DataExchange::BadFormatException on mixed content (text beside child elements), which this shape cannot
     *        hold - nor can a text member (Binding::Kind::eText), text beside attributes only: reading on would silently drop
     *        the text. And on text beside attributes, unless a Binding says which member it is (Binding::Kind::eText).
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
             *  \brief How the XML is parsed - by default, with Stroika's default provider
             */
            DataExchange::XML::Options fXMLOptions;

            /**
             *  \brief A schema the document is validated against as it is read: reading one that does not hold to it throws.
             *         nullptr (the default) validates nothing
             *
             *  Best compiled by the provider that reads (fXMLOptions) - @see DataExchange::XML::SAXParse.
             */
            DataExchange::XML::Schema::Ptr fValidateAgainst{nullptr};

            /**
             *  \brief Read what the Binding describes from a document that does not quite hold to it, rather than throw - as
             *         reading others' documents wants: UPnP devices', say, which are famously loose
             *
             *  Then an element or attribute the Binding does not declare is skipped, wherever it is - markup inside a leaf's
             *  text too; a member repeated that the Binding declares once keeps its last; text the Binding has nowhere for -
             *  in an object, an array, a presence marker - is passed over; and a value not of its declared type is absent.
             *
             *  The Writer writes none of these, so what it wrote reads the same either way: a complete Binding still
             *  round-trips every value of its type.
             */
            bool fLenient{false};
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
