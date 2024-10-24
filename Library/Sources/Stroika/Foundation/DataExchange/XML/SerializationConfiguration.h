/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_SerializationConfiguration_h_
#define _Stroika_Foundation_DataExchange_XML_SerializationConfiguration_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h"

/*
 * TODO:
 *      @todo   Add support for namespaces
 *
 *      @todo   Add support for Schema validation
 *              and optionally more stuff � for how to map between variant structure and schema.
 *              Maybe have virtual API to make certain decisions?
 *              Will need experience/experimentation to get right
 */

namespace Stroika::Foundation::DataExchange::XML {

    using Characters::String;

    /**
     *  @see XML::Reader, and @see XML::Writer
     */
    class SerializationConfiguration {
    public:
        SerializationConfiguration ();

    public:
        // NOTE - defaults to "Document"
        // if empty - then Write requiers
        //              Require (v.GetType () == VariantValue::eMap);
        //
        // If provided - must be a valid xml element name (ncname?)
        nonvirtual optional<String> GetDocumentElementName () const;
        nonvirtual void             SetDocumentElementName (const optional<String>& n);

    public:
        // NOTE - defaults to "Array"
        //
        // If provided - must be a valid xml element name (ncname?)
        //
        // DOCUMENT CONSEQUENCES OF MISSING (cannot deseriualize without knowing diff
        // between a single element and an array. Need some way to tell. Perhaps in the
        // the future this can be better handled infering data from the schema.
        // but for now - with the current implementation - this is needed.
        nonvirtual optional<String> GetArrayElementName () const;
        nonvirtual void             SetArrayElementName (const optional<String>& n);

    private:
        String fDocumentElementName_; // internally empty string for missing
        String fArrayElementName_;    // internally empty string for missing
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SerializationConfiguration.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_SerializationConfiguration_h_*/
