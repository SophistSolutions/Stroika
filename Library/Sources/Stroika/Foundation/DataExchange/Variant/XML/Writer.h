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
#include "Stroika/Foundation/DataExchange/XML/SerializationConfiguration.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

/*
 * TODO:
 *   @todo   Probably wrong, and certainly incomplete, but it is now at the point of being testable.
 *
 *   @todo   Add SerializationOptions (distingished from SerializationConfiguariton cuz thats used for
 *           both reader and writer).
 *           o   include?xml... header
 *           o   Pretty print (spacing/tabs etc)
 *           o   POSSIBLY option for characterset to write wiith?
 *
 *      @todo   fix thread-safety - cloning rep - lock for access config data
 */

namespace Stroika::Foundation::DataExchange::Variant::XML {

    using Characters::String;

    using DataExchange::XML::SerializationConfiguration;

    /**
     *  \brief Write a VariantValue as XML - an object's member is an element, an array is repeated elements
     *
     *  | VariantValue                  | XML |
     *  |-------------------------------|-----|
     *  | an object's member K          | the element `<K>` |
     *  | an array under member K       | an element per item: `<Array>` each, or - with SetArrayElementName (nullopt) - `<K>` repeated |
     *  | anything else                 | its element's text, as VariantValue::As<String> writes it (a date ISO 8601, a BLOB base64) |
     *  | null                          | an empty element |
     *
     *  \par Example Usage
     *      \code
     *          SerializationConfiguration config;
     *          config.SetDocumentElementName ("device"sv);
     *          config.SetArrayElementName (nullopt);           // <service> repeated, rather than <Array>
     *          Variant::XML::Writer{config}.Write (v, out);
     *      \endcode
     *      \code
     *          <device>
     *              <specVersion>
     *                  <major>1</major>
     *              </specVersion>
     *              <serviceList>
     *                  <service>a</service>
     *                  <service>b</service>
     *              </serviceList>
     *          </device>
     *      \endcode
     *
     *  \note The byte overload of Write writes a document, so it begins with an XML declaration naming the encoding it writes
     *        (UTF-8); the Character overload writes none, the caller owning the encoding, and perhaps writing this into a
     *        document of its own.
     *
     *  \note Attributes, namespaces and a given element order are not expressible yet - what a schema of someone else's needs.
     *        @see https://github.com/SophistSolutions/Stroika/issues/954
     *
     * The argument VariantValue must be composed of any combination of these types:
     *          o   VariantValue::eBoolean
     *          o   VariantValue::eInteger
     *          o   VariantValue::eFloat
     *          o   VariantValue::eString
     *          o   VariantValue::eMap
     *          o   VariantValue::eArray
     *  or it can be the type:
     *          o   VariantValue::eNull
     *
     *  Other types are illegal an XML and will trigger a 'Require' failure.
     */
    class Writer : public Variant::Writer {
    private:
        using inherited = Variant::Writer;

    private:
        class Rep_;

    public:
        Writer (const SerializationConfiguration& config = SerializationConfiguration{});

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
#include "Writer.inl"

#endif /*_Stroika_Foundation_DataExchange_Variant_XML_Writer_h_*/
