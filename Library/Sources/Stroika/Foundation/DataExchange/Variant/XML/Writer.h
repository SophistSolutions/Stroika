/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_XML_Writer_h_
#define _Stroika_Foundation_DataExchange_Variant_XML_Writer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/DataExchange/Variant/Writer.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

#include "Binding.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   The XML declaration (which the byte overload writes, and the Character overload does not) and its character
 *              set (UTF-8) as Options too, if ever wanted.
 */

namespace Stroika::Foundation::DataExchange::Variant::XML {

    using Characters::String;

    /**
     *  \brief Write a VariantValue as XML - an object's member is an element, an array is repeated elements
     *
     *  | VariantValue                  | XML |
     *  |-------------------------------|-----|
     *  | an object's member K          | the element `<K>` |
     *  | an array under member K       | `<K>`, wrapping an `<Array>` per item - or as the Binding's fItems or fDefaultArrayItems say: items of another name, or `<K>` repeated |
     *  | anything else                 | its element's text, as VariantValue::As<String> writes it (a date ISO 8601, a BLOB base64) |
     *  | null, or an empty object      | an empty element |
     *
     *  \par Example Usage
     *      \code
     *          // v is {"specVersion": {"major": 1}, "serviceList": {"service": ["a", "b"]}}
     *          // <service> repeated, not <Array>s
     *          Binding device{.fName = "device"sv, .fDefaultArrayItems = make_shared<Binding> (Binding::kRepeatedArrayItems)};
     *          Variant::XML::Writer{{.fBinding = device}}.WriteAsString (v);
     *      \endcode
     *      \code
     *          <device>
     *              <serviceList>
     *                  <service>a</service>
     *                  <service>b</service>
     *              </serviceList>
     *              <specVersion>
     *                  <major>1</major>
     *              </specVersion>
     *          </device>
     *      \endcode
     *      Members come out in whatever order the value's Mapping gives them (here serviceList first), a VariantValue object
     *      having no order of its own. A Binding gives them one (UPnP wants specVersion first).
     *
     *  \note The byte overload of Write writes a document, so it begins with an XML declaration naming the encoding it writes
     *        (UTF-8); the Character overload writes none, the caller owning the encoding, and perhaps writing this into a
     *        document of its own.
     *
     *  That is the shape with nothing declared. Where it is not the shape wanted - an attribute rather than an element, a
     *  namespace, a name of its own, an order of its own (XML cares about the order of an element's children, and a
     *  VariantValue object has none) - say so in a Binding, which the Reader needs as much as the Writer. @see Binding
     *  A member keyed "Array" needs one too, naming it, or it reads back as an array's item (@see Binding::fDefaultArrayItems).
     *
     *  **A Binding that declares a value's type (Binding::fType) writes it very differently** (fType's table): only a value of
     *  that type - anything else throws - an object only with the members its Binding declares, and a null member as none at
     *  all. With no type declared, any value is written, as it is.
     *
     *  **Given a complete Binding (Binding::IsComplete), the Reader reads back exactly what this wrote - or this throws
     *  BadFormatException, v not being one the Binding describes: never anything in between** (but for what XML writes as
     *  nothing - @see Binding::IsComplete).
     *
     *  \note A namespace is written as a default namespace (xmlns="..."), inherited by the elements inside it - or with a
     *        prefix, where a Binding's fNamespacePrefixes gives it one, which an attribute in a namespace requires. Mixed
     *        content, comments and processing instructions are still not expressible (and SAX reports no comment, so none
     *        could be read back anyway).
     *        @see https://github.com/SophistSolutions/Stroika/issues/954
     *
     *  \note Throws DataExchange::BadFormatException where v cannot be written as XML - v is data, not code:
     *          - a name that is not an XML name: a key such as "my key" or "1a", or a Binding's name or prefix
     *          - no single document element, a document having exactly one: with no Binding::fName, a v that is not an object of
     *            exactly one member (that member is the document element); or a document element that would be repeated, for an
     *            array of items with no name of their own
     *          - a value of another type than its Binding declares (Binding::fType), or a member of an object its Binding
     *            does not declare
     */
    class Writer : public Variant::Writer {
    private:
        using inherited = Variant::Writer;

    private:
        class Rep_;

    public:
        /**
         */
        struct Options {
            /**
             *  \brief How the value differs from the default shape - so the Reader must be given the same, to read it back
             */
            Binding fBinding;

            /**
             *  \brief An element to a line, indented by its depth; false: no whitespace between elements - nor after the XML
             *         declaration, nor at the end
             */
            bool fPrettyPrint{true};

            /**
             *  \brief Spaces per level of indentation - ignored unless fPrettyPrint
             */
            unsigned int fSpacesPerIndent{4};
        };

    public:
        /**
         *  \note Two constructors, not one with a default argument (= {}): within Writer, that default argument cannot use the
         *        member initializers of Options, nested in it (C++ CWG 1397) - MSVC allows it, clang and gcc do not.
         */
        Writer ();
        Writer (const Options& options); ///< \brief Writing as options says
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Writer.inl"

#endif /*_Stroika_Foundation_DataExchange_Variant_XML_Writer_h_*/
